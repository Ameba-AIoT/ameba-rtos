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

    def __init__(self, args) -> None:
        self.args = args
        self.logger = create_logger("imagetool")

        self.image_folder_name = 'image_mp' if args.mp == 'y' else 'image'
        self.image_output_dir = ''

        proj_info = parse_project_info(args.mcu_project_dir if args.mcu_project_dir else os.getcwd())

        self.soc_dir = proj_info['soc_dir']
        self.soc_project = args.soc_project if args.soc_project else proj_info['soc_project']
        self.mcu_project = args.mcu_project if args.mcu_project else proj_info['mcu_project']
        self.mcu_type = proj_info['mcu_type']

        self.manifest_file = os.path.join(self.soc_dir, 'manifest.json5')

        with open(self.manifest_file, 'r') as f:
            self.manifest_data = json5.load(f)

        self.layout_file = ''
        for name in ['ameba_layout.ld', f'{self.soc_project}_layout.ld', f'{self.soc_project.capitalize()}_layout.ld']:
            #FIXME: Uniform these name please!!
            layout_file = os.path.join(self.soc_dir, name)
            if os.path.exists(layout_file):
                self.layout_file = layout_file
                break
        if self.layout_file == '':
            self.logger.fatal("Failed get layout file for current project")
            raise ValueError

        # self.image_type = image_type(self, args.binary_files)

    def get_image_target_dir(self, mcu_project_name) -> str:
        mcu_type = mcu_project_to_mcu_type(mcu_project_name)
        if mcu_type:
            return os.path.join(
                self.soc_dir,
                f'project_{mcu_project_name}',
                'vsdk' if mcu_type == 'kr4' else 'asdk',
                self.image_folder_name
            )
        else:
            self.logger.fatal(f"Failed to get image target dir for mcu project: {mcu_project_name}")
            return ''

    def set_image_output_dir(self, path:str) -> None:
        self.image_output_dir = path

    def manifest_process(self):


        pass

