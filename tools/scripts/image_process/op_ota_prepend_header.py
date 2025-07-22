import os

from op_base import OperationBase
from context import Context
from utility import *
from ameba_enums import *

class OtaPrependHeader(OperationBase):
    cmd_help_msg = 'OTA prepend header'

    # Constant Variables
    version = 0xFFFFFFFF
    ota_num = 1
    ota_sign = 0x4F544131
    header_length = 24
    check_sum = 0
    offset = 32

    def __init__(self, context:Context) -> None:
        super().__init__(context)

        #NOTE: The order is important: image1 -> image2/app_all -> dsp
        # key:value <=> image_type:img_id
        self.image_list = {
            ImageType.IMAGE1: 0,
            ImageType.IMAGE2: 0xFF,  # skip update header, used in update_header
            ImageType.APP_ALL: 1,
            ImageType.DSP: 2,
        }
        self.images = {t:[] for t in self.image_list.keys()}
        self.header = b''

    @staticmethod
    def register_args(parser) -> None:
        parser.add_argument('-i', '--input-file', nargs='+', help='Input file to be process', required=True)
        parser.add_argument('-o', '--output-file', help='Output processed file', required=True)

    def pre_process(self) -> Error:
        for i in self.context.args.input_file:
            # check image file existence
            if not os.path.exists(i):
                self.logger.fatal(f'File not exist: {i}')
                return Error(ErrorType.FILE_NOT_FOUND, i)

            # classify image by type so the command line arg's order is not required
            image_type = parse_image_type(i)
            if image_type not in self.images.keys():
                self.logger.fatal(f'Unsupported image type: {image_type} for {i}')
                return Error(ErrorType.INVALID_INPUT)
            self.images[image_type].append(i)
        return Error.success()

    @exit_on_failure(catch_exception=True)
    def process(self) -> Error:
        self.ota_num = len(self.context.args.input_file)
        self.offset = 32 + 24 * (self.ota_num - 1)

        self.header = struct.pack('<II', self.version, self.ota_num)

        for img_type, img_id in self.image_list.items():
            for img in self.images[img_type]:
                self.update_header(img, img_id)

        full_images = []
        for t in self.image_list.keys():
            full_images.extend(self.images[t])
        merge_files(self.context.args.output_file, *full_images, header = self.header)

        offset = 16
        for img in full_images:
            with open(img, "rb") as f:
                content = f.read()
                checksum = sum(content)
                print(f"{os.path.basename(img):<25}{'image_size=':<10}{os.path.getsize(img):<10}{'checksum=':<9}{checksum:08x}")
                with open(self.context.args.output_file, "rb+") as f:
                    f.seek(offset, 0)
                    f.write(struct.pack('<I', checksum))
            offset += 24
        return Error.success()

    def post_process(self) -> Error:
        return Error.success()

    def update_header(self, image_file:str, image_id:int) -> None:
        image_size = os.path.getsize(image_file)
        if image_id != 0xFF:
            self.header += struct.pack('>I', self.ota_sign)
            self.header += struct.pack('<IIIII', self.header_length, self.check_sum, image_size, self.offset, image_id)
        self.offset += image_size