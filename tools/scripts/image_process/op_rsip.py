import os
import shutil
import importlib

from op_base import OperationBase
from context import Context
from utility import *
from ameba_enums import *
from manifest_manager import ManifestManager

class Rsip(OperationBase):
    cmd_help_msg = 'Rsip process image'

    def __init__(self, context:Context) -> None:
        super().__init__(context)

    @staticmethod
    def register_args(parser) -> None:
        parser.add_argument('-i', '--input-file', help='Input file to be process', required=True)
        parser.add_argument('-o', '--output-file', help='Output encrypted file', required=True)
        parser.add_argument('-s', '--section', help='Section in layout file', required=True)

    @staticmethod
    def require_manifest_file(context:Context) -> bool:
        return True

    @staticmethod
    def require_layout_file(context:Context) -> bool:
        return True

    # @exit_on_failure(catch_exception=True)
    @staticmethod
    def execute(context:Context, output_file:str, input_file:str, section:str, image_type:ImageType = ImageType.UNKNOWN):
        context.logger.info("process rsip file")
        layout_file = context.layout_file

        manifest_manager = ManifestManager(context)
        if image_type == ImageType.UNKNOWN:
            #Detect image type by image name
            image_type = parse_image_type(input_file)
        image_config = manifest_manager.get_image_config(image_type)
        if image_config.rsip_enable:
            section_addr = get_layout_address(layout_file, section, "ORIGIN")
            if section_addr == '':
                context.logger.fatal(f"Failed to parse addr for {section} in {layout_file}")
                return Error(ErrorType.INVALID_ARGS, f"Failed to parse addr for {section} in {layout_file}")
            lib_security = importlib.import_module('security')
            rsip = lib_security.RSIP(output_file, input_file, section_addr, image_config)
            if context.soc_project == "amebad":
                rsip.RSIP_ImageEncrypt_AMEBAD()
            else:
                rsip.RSIP_ImageEncrypt()
        else:
            shutil.copy(input_file, output_file)

        return Error.success()

    def pre_process(self) -> Error:
        layout_file = self.context.layout_file
        if not os.path.exists(layout_file):
            self.logger.fatal(f"layout file not exist: {layout_file}")
            return Error(ErrorType.FILE_NOT_FOUND, layout_file)
        return Error.success()

    def process(self) -> Error:
        return Rsip.execute(self.context, self.context.args.output_file, self.context.args.input_file, self.context.args.section)

    def post_process(self) -> Error:
        return Error.success()
