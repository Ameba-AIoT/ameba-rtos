import os
import click

from op_base import OperationBase
from context import Context
from ameba_enums import *
from utility import *

class BasedIntParamType(click.ParamType):
    name = 'integer'

    def convert(self, value, param, ctx):
        try:
            return int(value, 0)
        except ValueError:
            self.fail('%s is not a valid integer. Please use code literals '
                      'prefixed with 0b/0B, 0o/0O, or 0x/0X as necessary.'
                      % value, param, ctx)

class Pad(OperationBase):
    cmd_help_msg = 'BIN File Padding Tool: Pad specified hex value to the head/tail of a BIN file, supporting two padding length rules (alignment/fixed length)'

    def __init__(self, context:Context) -> None:
        super().__init__(context)

    @staticmethod
    def register_args(parser) -> None:
        parser.add_argument('-l', '--length', type=BasedIntParamType(),
                            help='Base padding length in bytes, works with --align to determine final file length rule',
                            required=True)
        parser.add_argument('-i', '--input-file',
                            help='Path to the BIN file to be processed (required)',
                            required=True)
        parser.add_argument('-v', '--value', type=BasedIntParamType(),
                            help='Padding value in hexadecimal, default 0xFF',
                            default="0xFF")
        parser.add_argument('-s', '--from-head', action='store_true',
                            help='Pad from the head of the file (default from tail)',
                            default=False)
        parser.add_argument('--no-align', action='store_true',
                            help='Donot align total file length to --length value after padding (enabled by default); enable to insert exactly --length bytes',
                            default=False)
        parser.add_argument('-o', '--output-file',
                            help='Path to the output file (optional). If specified, padded content is written to this file (original file remains unchanged); if not specified, modify the input file in-place',
                            default=None)

    @staticmethod
    def require_manifest_file(context:Context) -> bool:
        return False

    @staticmethod
    def require_layout_file(context:Context) -> bool:
        return False

    # @exit_on_failure(catch_exception=True)
    @staticmethod
    def execute(context:Context, input_file:str, length:int, value:int = 0xFF, from_head:bool = False, no_align:bool = True, output_file = None):
        context.logger.info("process pad file")
        file_size = os.path.getsize(input_file)
        if no_align:
            pad_count = length
        else:
            if file_size == 0:
                pad_count = 0
            else:
                new_size = ((file_size - 1) // length + 1) * length
                pad_count = new_size - file_size

        if pad_count <= 0:
            return Error.success()

        pad_byte = bytes([value & 0xFF])
        pad_data = pad_byte * pad_count
        if from_head:
            with open(input_file, "rb") as f:
                original_data = f.read()
            if output_file:
                with open(output_file, "wb") as f:
                    f.write(pad_data)
                    f.write(original_data)
            else:
                with open(input_file, "wb") as f:
                    f.write(pad_data)
                    f.write(original_data)
        else:
            if output_file:
                with open(input_file, "rb") as f:
                    original_data = f.read()
                with open(output_file, "wb") as f:
                    f.write(pad_data)
                    f.write(original_data)
            else:
                with open(input_file, "ab+") as f:
                    f.write(pad_data)
        return Error.success()

    def pre_process(self) -> Error:
        file_path = self.context.args.input_file
        if not os.path.exists(file_path):
            self.logger.fatal(f'File not exist: {file_path}')
            return Error(ErrorType.FILE_NOT_FOUND, file_path)

        return Error.success()

    def process(self) -> Error:
        return Pad.execute(self.context, self.context.args.input_file, self.context.args.length, self.context.args.value,
                           self.context.args.from_head, self.context.args.no_align, self.context.args.output_file)

    def post_process(self) -> Error:
        return Error.success()


