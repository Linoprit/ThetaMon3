import logging
from framework.settings import AppSettings as AppSettings
from IdTable.CommandMap import CommandMap
from framework.filehelper import Filehelper as Fh
from pathlib import Path
from IdTable.IdTableParser import IdTableParser as IdTableParser


class ParseRawIdFile:
    def __init__(self, settings_in: AppSettings):
        self.app_settings = settings_in
        cmd_map = CommandMap()
        file_to_read = (
            self.app_settings.settings)['table_id_tool']['raw_id_file']
        file_to_read = self.app_settings.expand(file_to_read)
        if file_to_read is None:
            logging.error("No file given, neither on commandline, nor in ini-file.")
            return

        path = Path(file_to_read)
        if not path.is_file():
            logging.error("Given file does not exist: {}".format(file_to_read))
            return

        id_file = Fh.read_file_to_list(path)
        id_parser = IdTableParser()
        id_parser.parse(id_file)
        id_table = id_parser.get_with_command(cmd_map.getCmdString('setSensIdCmd'))

        id_table_str = '\n'.join(id_table)
        out_file = (
            self.app_settings.settings)['table_id_tool']['id_table_outfile']
        out_file = self.app_settings.expand(out_file)
        Fh.dump_to_file(out_file, id_table_str)
        # print(id_table_str)
        # [print(i, end='\n') for i in id_table]
