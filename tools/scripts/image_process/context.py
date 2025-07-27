from abc import ABC
import os
import logging
import json5

from utility import *

class Context(ABC):
    manifest_file:str
    manifest_data:dict
    soc_project:str
    mcu_project:str
    mcu_type:str

    logger:logging.Logger

    def __init__(self, args, object) -> None:
        self.args = args
        self.logger = create_logger("imagetool")

        self.image_folder_name = 'image_mp' if args.mp == 'y' else 'image'
        self.image_output_dir = ''

        proj_info = parse_project_info(args.mcu_project_dir if args.mcu_project_dir else os.getcwd())

        self.soc_dir = proj_info['soc_dir']
        self.soc_project = args.soc_project if args.soc_project else proj_info['soc_project']
        self.mcu_project = args.mcu_project if args.mcu_project else proj_info['mcu_project']
        self.mcu_type = proj_info['mcu_type']

        if object.require_manifest_file(self):
            self.manifest_file = os.path.join(self.soc_dir, 'manifest.json5')
            if not os.path.exists(self.manifest_file):
                self.logger.fatal("Failed get manifest file for current project")
                raise ValueError
            with open(self.manifest_file, 'r') as f:
                self.manifest_data = json5.load(f)

        if object.require_layout_file(self):
            self.layout_file = os.path.join(self.soc_dir, 'ameba_layout.ld')
            if not os.path.exists(self.layout_file):
                self.logger.fatal("Failed get layout file for current project")
                raise ValueError
