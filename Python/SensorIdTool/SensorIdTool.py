import logging
import framework.settings
from ParseRawIdFile import ParseRawIdFile as ParseRawIdFile

if __name__ == '__main__':
    app_settings = framework.settings.AppSettings()
    app_settings.set_name_and_version('SensorIdTool', 'v000.001.001')
    app_settings.print_status()

    command = app_settings.get('common', 'command')
    if command == 'parse':
        ParseRawIdFile(app_settings)
    elif command == 'devel':
        print("devel called")
        pass
    else:
        logging.error("Unknown command: {}".format(command))
