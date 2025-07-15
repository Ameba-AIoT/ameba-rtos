import os
import shutil
import importlib

from op_base import OperationBase
from context import Context
from utility import *
from ameba_enums import *
from manifest_manager import ManifestManager

class Rdp(OperationBase):
    cmd_help_msg = 'Rdp process image'

    def __init__(self, context:Context) -> None:
        super().__init__(context)

    @staticmethod
    def register_args(parser) -> None:
        parser.add_argument('-m', '--mode', type=str, choices=['enc', 'dec'], default="enc", help='Encrypt file mode')
        parser.add_argument('-i', '--input-file', help='Input file to be process', required=True)
        parser.add_argument('-o', '--output-file', help='Output encrypted file', required=True)

    # @exit_on_failure(catch_exception=True)
    @staticmethod
    def execute(context:Context, output_file:str, input_file:str, mode = 'enc', image_type:ImageType = ImageType.UNKNOWN):
        context.logger.info("process rdp file")

        if image_type == ImageType.UNKNOWN:
            #Detect image type by image name
            image_type = parse_image_type(input_file)

        manifest_manager = ManifestManager(context)
        image_config = manifest_manager.get_image_config(image_type)
        if image_config.rdp_en:
            lib_security = importlib.import_module('security')
            rdp = lib_security.RDP(output_file, input_file, mode == 'enc', image_config)
            if context.soc_project == "amebad":
                 rdp.rdp_encrypt_AMEBAD()
            else:
                 rdp.rdp_encrypt()
        else:
            shutil.copy(input_file, output_file)
        return Error.success()

    def pre_process(self) -> Error:
        if not os.path.exists(self.context.args.input_file):
            self.logger.fatal(f"layout file not exist: {self.context.args.input_file}")
            return Error(ErrorType.FILE_NOT_FOUND, self.context.args.input_file)
        return Error.success()

    def process(self) -> Error:
        return Rdp.execute(
            self.context,
            self.context.args.output_file,
            self.context.args.input_file,
            self.context.args.mode,
            ImageType.IMAGE2 #WARNING: rdp only use config from image2 yet
        )

    def post_process(self) -> Error:
        return Error.success()


