import json
import shutil
import tempfile

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

        subparsers.add_parser('query-floader-sboot', help='Query effective image1 classical/PQC signing, prints ON or OFF')

        sub = subparsers.add_parser('strip-floader-manifest', help='Validate and strip an existing RLE1509 flashloader manifest')
        sub.add_argument('-i', '--input-file', help='Input flashloader with trailing 4KB or 8KB manifest', required=True)
        sub.add_argument('-o', '--output-file', help='Output IMAGE_HEADER and payload', required=True)

        #NOTE: args for cut-by-map
        sub = subparsers.add_parser('cut-by-map', help='Cut file by calculating size from map file symbols')
        sub.add_argument('-i', '--input-file', help='Input file', required=True)
        sub.add_argument('-o', '--output-file', help='Output file', required=True)
        sub.add_argument('-m', '--map-file', help='Map file', required=True)
        sub.add_argument('--start-sym', help='Start symbol', required=True)
        sub.add_argument('--end-sym', help='End symbol', required=True)

    @staticmethod
    def require_manifest_file(context:Context) -> bool:
        return context.args.sub_operation in ("manifest-fmt", "query-manifest", "query-floader-sboot")

    @staticmethod
    def require_layout_file(context:Context) -> bool:
        return False

    def pre_process(self) -> Error:
        if self.context.args.sub_operation == "merge":
            for f in self.context.args.input_file:
                if not os.path.exists(f):
                    return Error(ErrorType.FILE_NOT_FOUND, f)
        elif self.context.args.sub_operation in ("cut-by-map", "strip-floader-manifest"):
            if not os.path.exists(self.context.args.input_file):
                return Error(ErrorType.FILE_NOT_FOUND, self.context.args.input_file)
            if self.context.args.sub_operation == "cut-by-map" and not os.path.exists(self.context.args.map_file):
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
        elif self.context.args.sub_operation == "query-floader-sboot":
            return self.query_floader_sboot()
        elif self.context.args.sub_operation == "strip-floader-manifest":
            return self.strip_floader_manifest(self.context.args.input_file, self.context.args.output_file)
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

    @exit_on_failure(exit_on_failure=False, catch_exception=True)
    def query_floader_sboot(self) -> Error:
        if self.context.soc_project != 'RLE1509':
            return Error(ErrorType.INVALID_INPUT, 'Flashloader re-signing helper is only for RLE1509')
        # Keep stdout machine-readable even when optional image sections are absent.
        level = default_logger.level
        try:
            default_logger.setLevel(logging.WARNING)
            config = manifest_preprocess(self.context.manifest_data)['image1']
        finally:
            default_logger.setLevel(level)
        if type(config.get('version')) is not int or config['version'] not in (1, 2):
            return Error(ErrorType.INVALID_INPUT, 'RLE1509 image1 manifest version must be 1 or 2')
        classical = config.get('sboot_enable', config.get('secure_boot_en', False))
        # The packer selects PQC by algorithm, independently of classical signing.
        # A legacy enable request also reaches the packer's config validation, not a silent skip.
        pqc = config['version'] == 2 and (config.get('sboot_pqc_algorithm') or config.get('sboot_pqc_enable', False))
        print('ON' if classical or pqc else 'OFF')
        return Error.success()

    @exit_on_failure(exit_on_failure=False, catch_exception=True)
    def strip_floader_manifest(self, input_file:str, output_file:str) -> Error:
        from manifest_manager import Manifest_TypeDef, FlashCalibPattern, MANIFEST_SIZE_4K, MANIFEST_SIZE_8K

        if self.context.soc_project != 'RLE1509':
            return Error(ErrorType.INVALID_INPUT, 'Flashloader manifest stripping is only for RLE1509')
        if os.path.realpath(input_file) == os.path.realpath(output_file) or (os.path.exists(output_file) and os.path.samefile(input_file, output_file)):
            return Error(ErrorType.INVALID_ARGS, 'Input and output flashloader files must differ')
        with open(input_file, 'rb') as f:
            data = f.read()
        pattern = struct.pack('<2I', *FlashCalibPattern)
        # IMAGE_HEADER is 32 bytes; image_size at offset 8 excludes the header.
        if len(data) < 32 or data[:8] != pattern:
            return Error(ErrorType.INVALID_INPUT, 'Missing or invalid flashloader IMAGE_HEADER')
        payload_size = struct.unpack_from('<I', data, 8)[0]
        manifest_start = 32 + payload_size
        manifest_size = len(data) - manifest_start
        if not payload_size or manifest_size not in (MANIFEST_SIZE_4K, MANIFEST_SIZE_8K):
            return Error(ErrorType.INVALID_INPUT, 'Truncated payload or missing/invalid trailing flashloader manifest')
        manifest = Manifest_TypeDef.from_buffer_copy(data, manifest_start)
        if data[manifest_start:manifest_start + 8] != pattern or manifest.ImgID != 0 or manifest.ImgSize != manifest_start:
            return Error(ErrorType.INVALID_INPUT, 'Flashloader manifest does not match the IMAGE_HEADER payload boundary')
        if {1: MANIFEST_SIZE_4K, 2: MANIFEST_SIZE_8K}.get(manifest.Ver) != manifest_size:
            return Error(ErrorType.INVALID_INPUT, 'Flashloader manifest version/length mismatch')

        # Validate everything before replacing an existing output; never rewrite the source binary.
        temporary_file = None
        try:
            with tempfile.NamedTemporaryFile(dir=get_file_dir(output_file), delete=False) as f:
                temporary_file = f.name
                f.write(data[:manifest_start])
            os.replace(temporary_file, output_file)
        finally:
            if temporary_file and os.path.exists(temporary_file):
                os.remove(temporary_file)
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
