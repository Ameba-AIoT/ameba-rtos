import json

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

        #NOTE: args for cert
        sub = subparsers.add_parser('manifest-fmt', help='Format manifest file from json5 to json format')
        sub.add_argument('-o', '--output-file', help='Output certificate file', required=True)

    @staticmethod
    def require_manifest_file(context:Context) -> bool:
        return True

    @staticmethod
    def require_layout_file(context:Context) -> bool:
        return False

    def pre_process(self) -> Error:
        return Error.success()

    def process(self) -> Error:
        if self.context.args.sub_operation == "manifest-fmt":
            return self.manifest_format(
                self.context.args.output_file,
            )
        else:
            return Error(ErrorType.INVALID_INPUT)

    def post_process(self) -> Error:
        return Error.success()

    @exit_on_failure(catch_exception=True)
    def manifest_format(self, output_file:str) -> Error:
        with open(output_file, "w") as f:
            json.dump(manifest_preprocess(self.context.manifest_data), f, indent=2)
        return Error.success()
