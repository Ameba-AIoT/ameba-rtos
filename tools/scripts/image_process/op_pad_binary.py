import os

from op_base import OperationBase
from context import Context
from ameba_enums import *

class PadBinary(OperationBase):
    cmd_help_msg = 'Pad binary file to a given length'

    def __init__(self, context:Context) -> None:
        super().__init__(context)

    @staticmethod
    def register_args(parser) -> None:
        parser.add_argument('-l', '--length', type=int, help='Pad length in 1024 bytes', required=True)
        parser.add_argument('-i', '--input-file', help='Input file to be process', required=True)
        parser.add_argument('-o', '--output-file', help='Output processed file', required=True)

    @staticmethod
    def require_manifest_file(context:Context) -> bool:
        return False

    @staticmethod
    def require_layout_file(context:Context) -> bool:
        return False

    def pre_process(self) -> Error:
        file_path = self.context.args.input_file
        if not os.path.exists(file_path):
            self.logger.fatal(f'File not exist: {file_path}')
            return Error(ErrorType.FILE_NOT_FOUND, file_path)

        return Error.success()

    def process(self) -> Error:
        target_size = 1024 * self.context.args.length

        with open(self.context.args.input_file, 'rb') as src_file, open(self.context.args.output_file, 'wb') as dst_file:
            content = src_file.read(target_size)
            dst_file.write(content)

            if len(content) < target_size:
                padding = b'\x00' * (target_size - len(content))
                dst_file.write(padding)

        return Error.success()

    def post_process(self) -> Error:
        return Error.success()


