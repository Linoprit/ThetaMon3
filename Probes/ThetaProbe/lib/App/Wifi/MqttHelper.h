#ifndef MQTTHELPER_H
#define MQTTHELPER_H

//#pragma once
#include "Sensors/MeasurementPivot.h"
#include <Arduino.h>
#include <Config.h>
#include <IPAddress.h>
#include <WiFi.h>
#include <string>

namespace wifi {

class MqttHelper {
public:
  MqttHelper();
  virtual ~MqttHelper(){};

  void init(void);
  static MqttHelper &instance(void);
  void MqttSetup();

  void setMqttHost(IPAddress ipAddress) { _mqttHost = ipAddress; };
  void setMqttPort(uint16_t port) { _mqttPort = port; };

  void setMqttSpot(std::string spot) {
    copyMqttPath(_mqttPubSens, spot, (const char *)&MQTT_SENS,
                 sizeof(MQTT_SENS));
    copyMqttPath(_mqttPubLog, spot, (const char *)&MQTT_LOG, sizeof(MQTT_LOG));
    copyMqttPath(_mqttSubCmd, spot, (const char *)&MQTT_CMD, sizeof(MQTT_CMD));
  };

  void startWifi(std::string ssid, std::string pass) {
    WiFi.begin(ssid.c_str(), pass.c_str());
  }
  void stopWifi() { WiFi.disconnect(); }

  void pubishMeasurements(msmnt::MeasurementPivot *measurementPivot);
  int publishLog(uint8_t *message, uint16_t size);
  int publishLog(std::string message);
  void printMqttConf();
  void stopSerialPrint() { _DoSerialPrint = false; }
  void startSerialPrint() { _DoSerialPrint = true; }

  char *_mqttSubCmd;

private:
  IPAddress _mqttHost;
  uint16_t _mqttPort;
  char *_mqttPubSens;
  char *_mqttPubLog;
  bool _DoSerialPrint;

  void copyMqttPath(char *buffer, std::string spot, const char *mqttSuffix,
                    size_t mqttLen) {
    uint8_t spotLen = strlen(spot.c_str());

    if ((spotLen + mqttLen) > MQTT_PATHLEN) {
      spotLen = MQTT_PATHLEN - mqttLen;
      Serial.printf("\nWARNING: Spotname is too long. It will be truncated.\n");
    }

    memcpy(buffer, spot.c_str(), spotLen);
    memcpy(&buffer[spotLen], mqttSuffix, mqttLen);
  }
};
} // namespace wifi

#endif