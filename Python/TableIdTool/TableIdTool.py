import logging

import framework.settings
import tableIdTool.TableIdProgrammer as Tip


if __name__ == '__main__':
    settings = framework.settings.AppSettings()
    settings.set_name_and_version('ThetaMonitor', 'v000.001.001')
    settings.print_status()

    what_to_run = settings.get('common', 'tool_name')
    if what_to_run == 'prog':
        tip = Tip.TableIdProgrammer(settings)
    elif what_to_run == 'devel':
        pass
    else:
        logging.error("No tool_name specified.")


