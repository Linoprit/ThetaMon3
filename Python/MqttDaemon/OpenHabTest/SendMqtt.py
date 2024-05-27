import time
import paho.mqtt.client as mqtt
from random import randrange

# mosquitto_pub -t 'Lager/cmd' -m 'printMeasures'
# mosquitto_sub -h localhost -t Lager/sens
# systemctl start openhab.service


# MQTT broker details
broker_address = "192.168.178.101"
broker_port = 1883

# MQTT topic to publish messages to
topic_lager = "Lager/sens/ESP1Tmp"
topic_werkstatt = "Werkstatt/sens"


# Function to establish connection with the MQTT broker
def on_connect(client, userdata, flags, rc):
    if rc == 0:
        print("Connected to MQTT Broker")
    else:
        print("Failed to connect, return code %d\n", rc)


# Function to handle publish acknowledgements
def on_publish(client, userdata, mid):
    print("Message published")


# Create MQTT client instance
client = mqtt.Client()

# Assign callbacks
client.on_connect = on_connect
client.on_publish = on_publish

# Connect to MQTT broker
client.connect(broker_address, broker_port)

# Start the MQTT client loop
client.loop_start()

value = 0

# Publish messages periodically
try:
    while True:
        # Your message to be sent
        # message = '{"light": 5424, "moisture": 30, "temperature": 21.4, "conductivity": 1020, "battery": 100}'

        value_tmp = 4.5 + (randrange(10)/10)
        value_prs = 950 + (randrange(50))
        value_hum = 60 + (randrange(10))
        message_lager \
            = f'{{"TEMP":{value_tmp}}}'
            # = f'{{"name":"Lager", "temp":{value_tmp}, "prs":{value_prs}, "hum":{value_hum} }}'

        value_tmp = 25 + (randrange(15) / 10)
        value_prs = 850 + (randrange(80))
        value_hum = 50 + (randrange(15))
        message_werkstatt \
            = f'{{"name":"Werkstatt", "temp":{value_tmp}, "prs":{value_prs}, "hum":{value_hum} }}'
        # message = '{"value": 25}'

        # Publish the message to the topic
        client.publish(topic_lager, message_lager)
        client.publish(topic_werkstatt, message_werkstatt)


        # Print a confirmation message
        print(f"Sent: '{message_lager}' to topic '{topic_lager}'")
        print(f"Sent: '{message_werkstatt}' to topic '{topic_werkstatt}'")

        value += 1
        if value > 200:
            value = 1

        time.sleep(5)

except KeyboardInterrupt:
    # Stop the MQTT client loop
    client.loop_stop()
    print("MQTT client loop stopped")
