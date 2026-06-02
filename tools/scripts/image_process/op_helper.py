import json
import shutil

from op_base import OperationBase
from context import Context
from ameba_enums import *
from utility import *

class Helper(OperationBase):
    cmd_help_msg = 'Common helpers'

    def __init__(self, context:Context) -> None:
        super().__init__(context)

    @staticmethod
    def register_args(parser) -> None:
        subparsers = parser.add_subparsers(dest='sub_operation', help='Available operations for helper', required=True)

        #NOTE: args for manifest-fmt
        sub = subparsers.add_parser('manifest-fmt', help='Format manifest file from json5 to json format')
        sub.add_argument('-o', '--output-file', help='Output formatted file', required=True)

        #NOTE: args for merge
        sub = subparsers.add_parser('merge', help='Merge files to single one')
        sub.add_argument('-o', '--output-file', help='Output file', required=True)
        sub.add_argument('-i', '--input-file', nargs='+', help='Input files', required=True)

        #NOTE: args for query-manifest
        sub = subparsers.add_parser('query-manifest', help='Query a boolean key from manifest.json5, prints ON or OFF')
        sub.add_argument('--key', required=True, help='Key to query (e.g. sboot_enable)')

        #NOTE: args for cut-by-map
        sub = subparsers.add_parser('cut-by-map', help='Cut file by calculating size from map file symbols')
        sub.add_argument('-i', '--input-file', help='Input file', required=True)
        sub.add_argument('-o', '--output-file', help='Output file', required=True)
        sub.add_argument('-m', '--map-file', help='Map file', required=True)
        sub.add_argument('--start-sym', help='Start symbol', required=True)
        sub.add_argument('--end-sym', help='End symbol', required=True)

    @staticmethod
    def require_manifest_file(context:Context) -> bool:
        return context.args.sub_operation in ("manifest-fmt", "query-manifest")

    @staticmethod
    def require_layout_file(context:Context) -> bool:
        return False

    def pre_process(self) -> Error:
        if self.context.args.sub_operation == "merge":
            for f in self.context.args.input_file:
                if not os.path.exists(f):
                    return Error(ErrorType.FILE_NOT_FOUND, f)
        elif self.context.args.sub_operation == "cut-by-map":
            if not os.path.exists(self.context.args.input_file):
                return Error(ErrorType.FILE_NOT_FOUND, self.context.args.input_file)
            if not os.path.exists(self.context.args.map_file):
                return Error(ErrorType.FILE_NOT_FOUND, self.context.args.map_file)
        return Error.success()

    def process(self) -> Error:
        if self.context.args.sub_operation == "manifest-fmt":
            return self.manifest_format(
                self.context.args.output_file,
            )
        elif self.context.args.sub_operation == "merge":
            return self.merge(
                self.context.args.output_file,
                *self.context.args.input_file
            )
        elif self.context.args.sub_operation == "query-manifest":
            return self.query_manifest(self.context.args.key)
        elif self.context.args.sub_operation == "cut-by-map":
            return self.cut_by_map(
                self.context.args.input_file,
                self.context.args.output_file,
                self.context.args.map_file,
                self.context.args.start_sym,
                self.context.args.end_sym
            )
        else:
            return Error(ErrorType.INVALID_INPUT)

    def post_process(self) -> Error:
        return Error.success()

    @exit_on_failure(catch_exception=True)
    def query_manifest(self, key: str) -> Error:
        print('ON' if self.context.manifest_data.get(key, False) else 'OFF')
        return Error.success()

    @exit_on_failure(catch_exception=True)
    def manifest_format(self, output_file:str) -> Error:
        with open(output_file, "w") as f:
            json.dump(manifest_preprocess(self.context.manifest_data), f, indent=2)
        return Error.success()

    @exit_on_failure(catch_exception=True)
    def merge(self, output_file:str, *input_file) -> Error:
        with open(output_file, "wb") as out_f:
            for i in input_file:
                with open(i, 'rb') as in_f:
                    shutil.copyfileobj(in_f, out_f)
        return Error.success()

    @exit_on_failure(catch_exception=True)
    def cut_by_map(self, input_file:str, output_file:str, map_file:str, start_sym:str, end_sym:str) -> Error:
        from op_cut import Cut

        pad_start = parse_map_file(map_file, start_sym)
        pad_end = parse_map_file(map_file, end_sym)

        if not pad_start or not pad_end:
            self.context.logger.error(f"Error: Symbols {start_sym} or {end_sym} missing in map file")
            return Error(ErrorType.INVALID_INPUT)

        pad_len = int(pad_end[0], 16) - int(pad_start[0], 16)

        if pad_len > 0:
            return Cut.execute(self.context, input_file, output_file, pad_len)
        else:
            shutil.copy(input_file, output_file)
            return Error.success()
