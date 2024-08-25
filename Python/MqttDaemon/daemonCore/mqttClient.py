import logging

import paho.mqtt.client as mqtt
import framework.settings

global spotlist
global loglist


def on_connect(client, userdata, flags, rc):
    logging.info("Connected with result code " + str(rc))
    for spot in spotlist:
        client.subscribe((spot, 2))


def on_message(client, userdata, msg):
    # msg.topic
    if msg.payload.decode() == "Hello world!":
        print("Yes!")
        client.disconnect()


class MqttClient:
    def __init__(self, settings_in: framework.settings.AppSettings):
        self.app_settings = settings_in
        global spotlist
        spotlist = self.create_spotlist(self.app_settings.get('mqtt', 'mqtt_sens'))
        global loglist
        loglist = self.create_spotlist(self.app_settings.get('mqtt', 'mqtt_log'))

        client = mqtt.Client()
        mqtt_ip = self.app_settings.get('mqtt', 'mqtt_server')
        client.connect(mqtt_ip, 1883, 60)

        client.on_connect = on_connect
        client.on_message = on_message

        client.loop_forever()

    def create_spotlist(self, suffix):
        spots = self.app_settings.get('mqtt', 'mqtt_spots').split(",")
        spot_list = []
        for spot in spots:
            spot_list.append(spot + suffix)
        return spot_list
