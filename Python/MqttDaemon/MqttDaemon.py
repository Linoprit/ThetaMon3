import logging
import framework.settings
import daemonCore.mqttClient as mqttClient

if __name__ == '__main__':
    app_settings = framework.settings.AppSettings()
    app_settings.set_name_and_version('MqttDaemon', 'v000.001.001')
    app_settings.print_status()

    command = app_settings.get('common', 'command')
    if command == 'daemon':
        pass
        # ParseRawIdFile(app_settings)
    elif command == 'devel':
        mqtt = mqttClient.MqttClient(app_settings)

        pass
    else:
        logging.error("Unknown command: {}".format(command))
