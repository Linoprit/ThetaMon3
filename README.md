# ThetaMon3

ThetaMon3 is based on an ESP32 controller, which reads a bunch of sensors and sends the results to a MQTT-Broker, like Mosquitto. I use three probes, together with my Home Assistant installation to control the temperature in remote buildings.

<img src="Doc/Ressources/IMG_20240629_132323.jpg" alt="drawing" width="300"/>

## Features Overview

- Reads DS18B20 sensor array, connected to two channels. In the current configuration, it can read up to four sensors per channel. If you need more, simply increase DS18B20_MAX_DEVICES in Config.h.
- It also reads BME280 values, temp, hummidity and pressure.
- Currently, every channel needs at least one sensor. So a minimum configuration is two DS18B20 sensors and one BME280. 
- Based on a look-up table, placed as file image on the controller, two relays are switched, depending on the given thresholds.
- Measurements are taken every minute and the average of these measurements are sent to an Mqtt broker every five minutes.
- There is a command-line-interface, you could connect to with a terminal programm, like GTK-Term. So you can have insight, what is going on in your setup and you can change several settings with it.
- Also, there are Mqtt-channels for remote accessing the device, or read loggings.

Why two OneWire-channels?<br>
I simply had the problem that in the workshop the location of the probe was in the middle of the sensor chain. Unfortunately, it was not possible to place it at one end or the other of the sensor chain.

## What's it all about? 

To understand what ThetaMon does and what it is meant for, we take a look at where the idea comes from.

My wife owns really a lot of pot-plants. These must be kept in a warmer place during the winter time, so we bring them into the garages. Unfortunately, on some days the outside temperatures could drop so low, that even in the garage the plants will die. Of course you use an electric heater, but the control of such devices is not particularly precise.
So the idea came up to build a system, that measures at different places in the room, to bring more insight how the heat is distributed and which places are save. Above that, to provide a finer regulation of the heating.

I put together the first version of ThetaMon, based on STM32F103 controllers, it consisted of two satellites and a base-station. The communication was done with NRF24 radios, which are long range capable (more or less...). The base-station was connected to an orange-pi which ran an apache webserver installation and hosted a page to show the graphs of all sensors for the last 24 hours. One very crucial point was, that the satellites work independently. If the connection to the base broke, they must capable to do the regulation. This means, that every node in the system has to keep a table with all relevant temperature-thresholds.

This system ran fine for some years, but I wanted more flexibility and insight if things went wrong. Sometimes temperature sensors failed, due to connector problems, or the NRF24 modules failed.
ThetaMon2 took a very long time to develop, and it became very complex. For example I needed an EEPROM with some kind of file-system to store the sensor tables. The html-generation was done by own code on the used Banana-Pi. Finaly it worked and it was used for one year.

In winter 2023 I took the time to try, if ESP32 with external antenna could keeo a stable connection to the Wlan. And yes, it did! Also I looked for some different possibility to do the development and found, that PlatformIO is so much powerfull. Then, a colleague showed me what he does with home automation and I came up to use Home Assistant as dashboard solution.
ThetaMon3 took only a half year of development, not only because large parts of code from TM2 have been overtaken, it turned out that PlatformIO absolutely speeds up things.

## Getting started

Refer to: [SettingUpThetaMonProbe](https://github.com/Linoprit/ThetaMon3/blob/main/Doc/SettingUpThetaMonProbe.pdf).<br>
You need a working Visual Studio Code, together with PlatformIO. 

For a complete command-reference see [CommandLineReference](https://github.com/Linoprit/ThetaMon3/blob/main/Doc/CommandLineReference.pdf).

## Integrating to Home Assistant

The probes data is transmitted via mqtt, so you could integrate it whereever you want.
Because I use HA, I describe the integration for it. 

The name of your location (spot), where the probe is placed is defined in „MqttConfig.txt“, that resides on your ESP. Every sensor has a shortname, defined in „ID_Table_U64.txt“. These two names result in a mqtt publishing, in the form \<Location>/sens/\<Sensor-Shortname>. Your Home Automation system has to subscribe every sensor. The sensor data is transmitted in Json-format, i.e. "{"HUMID":51.21}"

Your Mosquitto installation and setup should have been done in the last step. You could use Home Assistants Mosquitto plugin.<br>
Go to the file /homeassistant/configuration.yaml, use the text-editor-plugin. 
Create a new section 
```
mqtt:
    sensor:
```
Then for every sensor enter a block like this:
```
    - name: "<Location> <Sensor-Shortname>"
      unique_id: <Location><Sensor-Shortname>
      state_class: "measurement"
      state_topic: "<Location>/sens/<Sensor-Shortname>"
      unit_of_measurement: "°C"
      expire_after: 480
      value_template: "{{ value_json.TEMP }}"
```
### Unit and value_template for other sensors:

Pressure:
```
      unit_of_measurement: "hPa"
      value_template: "{{ value_json.PRESS }}"
```

Hummidity:
```
      unit_of_measurement: "%"
      value_template: "{{ value_json.HUMID }}"
```

Relay:
```
      unit_of_measurement: "0/1"
      expire_after: 480
      value_template: "{{ value_json.RELAY }}"
```

Example configuration:
```
# -- ESP32 Sensors --
mqtt:
    sensor:    
    - name: "Lager Temperatur"
      unique_id: LagerTmp
      state_class: "measurement"
      state_topic: "Lager/sens/LagerTmp"
      unit_of_measurement: "°C"
      expire_after: 480
      value_template: "{{ value_json.TEMP }}"
    - name: "Lager Luftdruck"
      unique_id: LagerPrs
      state_class: "measurement"
      state_topic: "Lager/sens/LagerPrs"
      unit_of_measurement: "hPa"
      expire_after: 480
      value_template: "{{ value_json.PRESS }}"
    - name: "Lager Luftfeuchte"
      unique_id: LagerHum
      state_topic: "Lager/sens/LagerHum"
      state_class: "measurement"
      unit_of_measurement: "%"
      expire_after: 480
      value_template: "{{ value_json.HUMID }}"
    - name: "Lager  Relay 1"
      unique_id: LagerRl1
      state_topic: "Lager/sens/LagerRl1"
      state_class: "measurement"
      unit_of_measurement: "0/1"
      expire_after: 480
      value_template: "{{ value_json.RELAY }}"
    - name: "Lager  Relay 2"
      unique_id: LagerRl2
      state_topic: "Lager/sens/LagerRl2"
      state_class: "measurement"
      unit_of_measurement: "0/1"
      expire_after: 480
      value_template: "{{ value_json.RELAY }}"
```

## Read Probe-Logging
Every probe publishes it's logging. It can be monitored with
```
mosquitto_sub -u mosquitto -P public -h <server-ip> -t <Location>/log
```

## Commands from remote
Via Mosquitto commands can be sent to your probe with
```
mosquitto_pub -t '<Location>/cmd' -m '<Command>'
```
Best practise is to have a second terminal opened with a running log-subscription.

## ToDo

- Command-Line reference, from file Interpreter.cpp
- How TM3 works. Measurement_Pivot, CommLine-Interface...?