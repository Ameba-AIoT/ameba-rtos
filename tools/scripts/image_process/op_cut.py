import os

from op_base import OperationBase
from context import Context
from ameba_enums import *

class Cut(OperationBase):
    cmd_help_msg = 'Cut a file to remove the given length'

    def __init__(self, context:Context) -> None:
        super().__init__(context)

    @staticmethod
    def register_args(parser) -> None:
        parser.add_argument('-i', '--input-file', help='Input file to be processed', required=True)
        parser.add_argument('-o', '--output-file', help='Output file', required=True)
        parser.add_argument('-l', '--length', type=int, help='Length in bytes to remove', required=True)
        parser.add_argument('-B', '--backward', action='store_true', help='Cut from the end of the file', default=False)
        parser.add_argument('-O', '--offset', type=int, help='Offset bytes from the cutting direction, default=0', default=0)

    @staticmethod
    def require_manifest_file(context:Context) -> bool:
        return False

    @staticmethod
    def require_layout_file(context:Context) -> bool:
        return False

    @staticmethod
    def execute(context: Context, input_file: str, output_file: str, length: int, backward: bool = False, offset: int = 0) -> Error:
        # Cut from the head:
        # ├────────┤──────────────────┤─────────┤
        # │ offset │ length to remove │         │
        # ├────────┤──────────────────┤─────────┤
        #
        # Cut from the end (backward):
        # ├────────┤──────────────────┤─────────┤
        # │        │ length to remove │  offset │
        # ├────────┤──────────────────┤─────────┤

        try:
            with open(input_file, "rb") as file:
                data = file.read()

            file_size = len(data)

            if length < 0 or length >= file_size:
                raise ValueError(f"Length must be [0, {file_size})")
            if offset < 0 or offset >= file_size:
                raise ValueError(f"Offset must be [0, {file_size})")
            if offset + length > file_size:
                raise ValueError(f"Offset ({offset}) + length ({length}) exceeds file size ({file_size})")

            if backward:
                context.logger.info(f"Cutting file: Backward, offset {offset}, length {length}")
                right = file_size - offset
                left = file_size - offset - length
            else:
                context.logger.info(f"Cutting file: Forward, offset {offset}, length {length}")
                left = offset
                right = offset + length

            result = data[:left] + data[right:]

            with open(output_file, "wb") as file:
                file.write(result)

            context.logger.info(f"File cut successfully.")
            return Error.success()

        except ValueError as e:
            err_msg = f"Invalid argument: {str(e)}"
            context.logger.error(err_msg)
            return Error(ErrorType.INVALID_ARGS)
        except Exception as e:
            err_msg = f"Unexpected error: {str(e)}"
            context.logger.error(err_msg)
            return Error(ErrorType.UNKNOWN_ERROR)

    def pre_process(self) -> Error:
        if not os.path.exists(self.context.args.input_file):
            self.logger.fatal(f'Input file not exist: {self.context.args.input_file}')
            return Error(ErrorType.FILE_NOT_FOUND, self.context.args.input_file)
        return Error.success()

    def process(self) -> Error:
        return Cut.execute(
            self.context,
            self.context.args.input_file,
            self.context.args.output_file,
            self.context.args.length,
            self.context.args.backward,
            self.context.args.offset
        )

    def post_process(self) -> Error:
        return Error.success()
