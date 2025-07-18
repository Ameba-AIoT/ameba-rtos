import os

from op_base import OperationBase
from context import Context
from ameba_enums import *
from utility import *

class Pad(OperationBase):
    cmd_help_msg = 'Pad binary file to align a given length'

    def __init__(self, context:Context) -> None:
        super().__init__(context)

    @staticmethod
    def register_args(parser) -> None:
        parser.add_argument('-l', '--length', type=int, help='Pad length in bytes', required=True)
        parser.add_argument('-i', '--input-file', help='Input file to be process', required=True)

    # @exit_on_failure(catch_exception=True)
    @staticmethod
    def execute(context:Context, input_file:str, length:int):
        context.logger.info("process pad file")
        file_size = os.path.getsize(input_file)
        if file_size == 0:
            pad_count = 0
        else:
            new_size = ((file_size - 1) // length + 1) * length
            pad_count = new_size - file_size

        with open(input_file, "ab+") as file:
            file.write(b'\xFF' * pad_count)
        return Error.success()

    def pre_process(self) -> Error:
        file_path = self.context.args.input_file
        if not os.path.exists(file_path):
            self.logger.fatal(f'File not exist: {file_path}')
            return Error(ErrorType.FILE_NOT_FOUND, file_path)

        return Error.success()

    def process(self) -> Error:
        return Pad.execute(self.context, self.context.args.input_file, self.context.args.length)

    def post_process(self) -> Error:
        return Error.success()


