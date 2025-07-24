import os
from typing import Union

from op_base import OperationBase
from context import Context
from utility import *
from ameba_enums import *

class PrependHeader(OperationBase):
    cmd_help_msg = 'Prepend header'

    # Constant Variables
    pattern_1=0x99999696
    pattern_2=0x3FCC66FC
    pattern_fs_1=0x7666735F
    pattern_fs_2=0x6661745F
    reserved=0xFFFFFFFFFFFFFFFF
    img2sign=0x3831393538373131

    def __init__(self, context:Context) -> None:
        super().__init__(context)

    @staticmethod
    def register_args(parser) -> None:
        parser.add_argument('-m', '--map-file', help='Map file of axf file')
        parser.add_argument('-s', '--symbol', help='Symbol to be process')
        parser.add_argument('-i', '--input-file', help='Input file to be process', required=True)
        parser.add_argument('-o', '--output-file', help='Output processed file', required=True)
        parser.add_argument('--boot-index', type=lambda x: int(x, 0), help='Boot index')

    @staticmethod
    def require_manifest_file(context:Context) -> bool:
        return False

    @staticmethod
    def require_layout_file(context:Context) -> bool:
        return False

    # @exit_on_failure(catch_exception=True)
    @staticmethod
    def execute(context:Context, output_file:str, input_file:str, map_file:str = '', symbol:str = '', boot_index:Union[int, None] = None):
        file_name = os.path.basename(input_file)
        file_size = os.path.getsize(input_file)
        if map_file:
            symbol_info = parse_map_file(map_file, symbol)
        else:
            symbol_info = ('0', '?', '')

        header = ''
        if file_name in ['ram_1.bin', 'xip_boot.bin', 'entry_1.bin']:
            header = PrependHeader.pattern_1.to_bytes(4, 'big') +  PrependHeader.pattern_2.to_bytes(4, 'big')
        elif file_name == 'fatfs.bin':
            header = PrependHeader.pattern_fs_1.to_bytes(4, 'big') +  PrependHeader.pattern_fs_2.to_bytes(4, 'big')
        else:
            header = PrependHeader.img2sign.to_bytes(8, 'big')
        header += file_size.to_bytes(4, 'little') + int(symbol_info[0], 16).to_bytes(4, 'little')

        if boot_index != None:
            header += boot_index.to_bytes(4, 'little') + 0xFFFFFFFF.to_bytes(4, 'little')
        elif file_name == 'fatfs.bin':
            pad_count = 0x1000 - 0x18
            for _ in range(pad_count):
                header += (b'\xFF')
        else:
            header += PrependHeader.reserved.to_bytes(8, 'little')
        header += PrependHeader.reserved.to_bytes(8, 'little')

        merge_files(
            output_file,
            input_file,
            header=header
        )
        return Error.success()

    def pre_process(self) -> Error:
        if self.context.args.map_file is None != self.context.args.symbol is None:
            self.logger.fatal(f'--map-file and --symbol must be specified together or not specified at all')
            return Error(ErrorType.INVALID_INPUT)
        return Error.success()

    def process(self) -> Error:
        return PrependHeader.execute(
            self.context,
            self.context.args.output_file,
            self.context.args.input_file,
            self.context.args.map_file,
            self.context.args.symbol,
            self.context.args.boot_index
        )


    def post_process(self) -> Error:
        return Error.success()
