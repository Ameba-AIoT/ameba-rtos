import shutil
import importlib
import json

from op_base import OperationBase
from context import Context
from ameba_enums import *
from utility import *
from manifest_manager import ManifestManager

class Encrypt(OperationBase):
    cmd_help_msg = 'Encrypt binary file'

    def __init__(self, context:Context) -> None:
        super().__init__(context)
        self.manifest_manager = ManifestManager(context)

    @staticmethod
    def register_args(parser) -> None:
        subparsers = parser.add_subparsers(dest='sub_operation', help='Available operations for encrypt', required=True)

        #NOTE: args for cert
        sub = subparsers.add_parser('cert', help='Create certificate file')
        sub.add_argument('-o', '--output-file', help='Output certificate file', required=True)
        sub.add_argument('--entry-pairs', nargs='+', help='Certificate entry paris', required=True)

        #NOTE: args for manifest
        sub = subparsers.add_parser('manifest', help='Create manifest file')
        sub.add_argument('-i', '--input-file', help='Input file to be process', required=True)
        sub.add_argument('-o', '--output-file', help='Output manifest file', required=True)

        #NOTE: args for keypair
        sub = subparsers.add_parser('keypair', help='Create manifest file')
        sub.add_argument('-a', '--algorithm', type=str, choices=ManifestManager.valid_algorithm, help='Algorithm to generate key pair', required=True)
        sub.add_argument('-o', '--output-file', help='Output key pair file', required=True)

    def pre_process(self) -> Error:
        return Error.success()

    def process(self) -> Error:
        if self.context.args.sub_operation == "cert":
            return self.create_cert(
                self.context.args.output_file,
                *self.context.args.entry_pairs
            )
        elif self.context.args.sub_operation == "manifest":
            return self.create_manifest(
                self.context.args.output_file,
                self.context.args.input_file
            )
        elif self.context.args.sub_operation == "keypair":
            return self.create_keypair(
                self.context.args.output_file,
                self.context.args.algorithm
            )
        else:
            return Error(ErrorType.INVALID_INPUT)

    def post_process(self) -> Error:
        return Error.success()

    @exit_on_failure(catch_exception=True)
    def create_cert(self, output_file:str, *entry_pairs) -> Error:
        self.logger.info(f"create cert file: {output_file}")
        return self.manifest_manager.create_cert(output_file, *entry_pairs)

    @exit_on_failure(catch_exception=True)
    def create_manifest(self, output_file:str, input_file:str) -> Error:
        self.logger.info(f"create manifest file for {input_file}")
        return self.manifest_manager.create_manifest(output_file, input_file)

    @exit_on_failure(catch_exception=True)
    def create_keypair(self, output_file:str, algorithm:str) -> Error:
        self.logger.info(f"create keypair file for {algorithm}")
        return self.manifest_manager.create_keypair(output_file, algorithm)
