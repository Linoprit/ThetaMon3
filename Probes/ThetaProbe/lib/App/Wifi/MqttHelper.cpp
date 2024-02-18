#include "CommandLine/CommandLine.h"
#include "MqttHelper.h"
#include "Sensors/Sensors.h"
#include "Wifi/MqLog.h"
#include <AppTypes.h>
#include <Arduino.h>
#include <AsyncMqttClient.h>
#include <DigitalIo/GpioInOut.h>
#include <FileSystem/LittleFsHelpers.h>
#include <WiFi.h>
#include <OsHelpers.h>

namespace wifi {

using namespace msmnt;

/// begin Mqtt callbacks and timer
AsyncMqttClient mqttClient;
TimerHandle_t mqttReconnectTimer;
TimerHandle_t wifiReconnectTimer;

void connectToMqtt() {
  MqLog("Connecting to MQTT...\n");
  mqttClient.connect();
}

void WiFiEvent(WiFiEvent_t event) {
  // Serial.printf("[WiFi-event] event: %d\n", event);
  switch (event) {
  case SYSTEM_EVENT_STA_GOT_IP:
    MqLog("\nWIFI is connected, local IP: %i.%i.%i.%i\n", WiFi.localIP()[0],
          WiFi.localIP()[1], WiFi.localIP()[2], WiFi.localIP()[3]);
    connectToMqtt();
    break;
  case SYSTEM_EVENT_STA_DISCONNECTED:
    MqLog("WiFi lost connection\n");
    xTimerStop(
        mqttReconnectTimer,
        0); // ensure we don't reconnect to MQTT while reconnecting to Wi-Fi
    xTimerStart(wifiReconnectTimer, 0);
    break;
  }
}

void onMqttConnect(bool sessionPresent) {
  MqLog("Connected to MQTT. Session present: %i\n", sessionPresent);

  uint16_t result =
      mqttClient.subscribe(wifi::MqttHelper::instance()._mqttSubCmd, 1);

  MqLog("Subscribing to '%s', got ID %lu\n",
        wifi::MqttHelper::instance()._mqttSubCmd, result);

  gpio::GpioInOut::instance().setLedConnected();
}

void onMqttDisconnect(AsyncMqttClientDisconnectReason reason) {
  MqLog("Disconnected from MQTT.\n");
  if (WiFi.isConnected()) {
    xTimerStart(mqttReconnectTimer, 0);
  }
  gpio::GpioInOut::instance().clrLedConnected();
}

void onMqttPublish(uint16_t packetId) { MqLog("Pub ack. Id: %lu\n", packetId); }

void connectToWifi() {
  MqLog("Reconnecting to Wi-Fi...\n");
  nvm::LittleFsHelpers::instance().readMqttConf();
  // WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
}

void onMqttMessage(char *topic, char *payload,
                   AsyncMqttClientMessageProperties properties, size_t len,
                   size_t index, size_t total) {

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
  _DoSerialPrint = true;

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
  if (WiFi.status() == WL_CONNECTED) {
    MqLog("\nWIFI is connected, local IP: %i.%i.%i.%i\n", WiFi.localIP()[0],
          WiFi.localIP()[1], WiFi.localIP()[2], WiFi.localIP()[3]);
  } else {
    MqLog("\nWifi is not  connected.\n");
  }
  MqLog("MqttHost: %i.%i.%i.%i:%lu\n", _mqttHost[0], _mqttHost[1], _mqttHost[2],
        _mqttHost[3], _mqttPort);
  MqLog("Mqtt publish sensordata: '%s'\n", _mqttPubSens);
  MqLog("Mqtt publish logs: '%s'\n", _mqttPubLog);
  MqLog("Mqtt subscribe commands: '%s'\n", _mqttSubCmd);
}

void MqttHelper::pubishMeasurements(MeasurementPivot *measurementPivot) {
  uint8_t publishCount = 0;

  // uint64: 20, float: 6, spaces: 1, EOL: 1
  char buff[30];

  measurementPivot->ResetIter();
  Measurement *actMeasurement = measurementPivot->GetNextMeasurement();

  while (actMeasurement != nullptr) {
    if ((!actMeasurement->isTimeout()) &&
        (!isnanf(actMeasurement->meanValue))) {
      sprintf(buff, "%llu %.02f;\0", actMeasurement->sensorId,
              actMeasurement->meanValue);

      uint16_t packetIdPub1 = mqttClient.publish(_mqttPubSens, 1, true, buff);
      publishCount++;
      // Serial.printf("Pub on topic '%s' at QoS 1, Id: %lu\n", _mqttPubSens,
      //               packetIdPub1);
    }
    actMeasurement = measurementPivot->GetNextMeasurement();
  }
  MqLog("(%lu) Published %i sensors.\n", OsHelpers::GetTickSeconds(), publishCount);
}

int MqttHelper::publishLog(uint8_t *message, uint16_t size) {
  if (_DoSerialPrint) {
    Serial.printf((char *)message);
  }

  uint16_t packetIdPub1 =
      mqttClient.publish(_mqttPubLog, 1, true, (char *)message);
  if (packetIdPub1 == 0) {
    return _FAIL_;
  }
  return _SUCCESS_;
}

int MqttHelper::publishLog(std::string message) {
  return publishLog((uint8_t *)message.c_str(), message.length());
}

void MqttHelper::copyMqttPath(char *buffer, std::string spot,
                              const char *mqttSuffix, size_t mqttLen) {
  uint8_t spotLen = strlen(spot.c_str());

  if ((spotLen + mqttLen) > MQTT_PATHLEN) {
    spotLen = MQTT_PATHLEN - mqttLen;
    MqLog("\nWARNING: Spotname is too long. It will be truncated.\n");
  }

  memcpy(buffer, spot.c_str(), spotLen);
  memcpy(&buffer[spotLen], mqttSuffix, mqttLen);
}

} // namespace wifi