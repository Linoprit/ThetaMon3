#include "CommandLine/CommandLine.h"
#include "MqttHelper.h"
#include "Sensors/Sensors.h"
#include <AppTypes.h>
#include <Arduino.h>
#include <AsyncMqttClient.h>
#include <FileSystem/LittleFsHelpers.h>
#include <WiFi.h>
#include "Wifi/MqLog.h"

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
  // Serial.printf("[WiFi-event] event: %d\n", event);
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
  Serial.printf("Connected to MQTT. Session present: %i\n", sessionPresent);

  uint16_t result =
      mqttClient.subscribe(wifi::MqttHelper::instance()._mqttSubCmd, 1);

  Serial.printf("Subscribing to '%s', got ID %lu\n",
                wifi::MqttHelper::instance()._mqttSubCmd, result);
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

void onMqttMessage(char *topic, char *payload,
                   AsyncMqttClientMessageProperties properties, size_t len,
                   size_t index, size_t total) {
  // Serial.println("Publish received.");
  // Serial.print("  topic: ");
  // Serial.println(topic);
  // Serial.print("  qos: ");
  // Serial.println(properties.qos);
  // Serial.print("  dup: ");
  // Serial.println(properties.dup);
  // Serial.print("  retain: ");
  // Serial.println(properties.retain);
  // Serial.print("  len: ");
  // Serial.println(len);
  // Serial.print("  index: ");
  // Serial.println(index);
  // Serial.print("  total: ");
  // Serial.println(total);
  // Serial.printf("Message: '%s'\n", payload);

  for (uint_fast8_t i = 0; i < total; i++) {
    xQueueSendToBack(keyBufferQueue, &payload[i], 50);
  }
  if (payload[total - 1] != cLine::_KEY_ENTER) {
    uint8_t chr = cLine::_KEY_ENTER;
    xQueueSendToBack(keyBufferQueue, &chr, 50);
  }
}
/// end Mqtt callbacks and timer

void MqttHelper::init(void) { new (&instance()) MqttHelper(); }

MqttHelper &MqttHelper::instance(void) {
  static MqttHelper mqttHelper;
  return mqttHelper;
}

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
  mqttClient.onMessage(onMqttMessage);

  // Connection to Wifi is done by reading the MqttConfig file.
  nvm::LittleFsHelpers::instance().readMqttConf();

  mqttClient.setServer(_mqttHost, _mqttPort);
  // If your broker requires authentication (username and password), set them
  // below mqttClient.setCredentials("REPlACE_WITH_YOUR_USER",
  // "REPLACE_WITH_YOUR_PASSWORD");

  WiFi.onEvent(WiFiEvent);
}

void MqttHelper::printMqttConf() {
  int result;

  if (WiFi.status() == WL_CONNECTED) {    
    result = MqLog("\nWIFI is connected, local IP: %i.%i.%i.%i\n",
                  WiFi.localIP()[0], WiFi.localIP()[1], WiFi.localIP()[2],
                  WiFi.localIP()[3]);

  } else {
    result = MqLog("\nWifi is not  connected.\n");
  }
  MqLog("MqttHost: %i.%i.%i.%i:%lu\n", _mqttHost[0], _mqttHost[1],
                _mqttHost[2], _mqttHost[3], _mqttPort);
  MqLog("Mqtt publish sensordata: '%s'\n", _mqttPubSens);
  MqLog("Mqtt publish logs: '%s'\n", _mqttPubLog);
  MqLog("Mqtt subscribe commands: '%s'\n", _mqttSubCmd);
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
    // Serial.printf("Pub on topic '%s' at QoS 1, Id: %lu\n", _mqttPubSens,
    //               packetIdPub1);

    actMeasurement = measurementPivot->GetNextMeasurement();
  }
}

int MqttHelper::PublishLog(uint8_t *message, uint16_t size) {
  message[size] = '\0';
  Serial.printf((char *)message);
  uint16_t packetIdPub1 =
      mqttClient.publish(_mqttPubLog, 1, true, (char *)message);
  if (packetIdPub1 == 0) {
    return _FAIL_;
  }
  return _SUCCESS_;
}

int MqttHelper::PublishLog(std::string message) {
  return PublishLog((uint8_t *)message.c_str(), message.length());
}

} // namespace wifi