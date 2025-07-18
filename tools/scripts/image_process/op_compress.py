import os
import sys
import lzma

from op_base import OperationBase
from context import Context
from ameba_enums import *
from manifest_manager import ManifestManager
from utility import *

class Compress(OperationBase):
    cmd_help_msg = 'Pad binary file to align a given length'

    def __init__(self, context:Context) -> None:
        super().__init__(context)
        self.manifest_manager = ManifestManager(context)

    @staticmethod
    def register_args(parser) -> None:
        parser.add_argument('-i', '--input-file', help='Input file to be process', required=True)
        parser.add_argument('-o', '--output-file', help='Output processed file', required=True)

    def pre_process(self) -> Error:
        file_path = self.context.args.input_file
        if not os.path.exists(file_path):
            self.logger.fatal(f'File not exist: {file_path}')
            return Error(ErrorType.FILE_NOT_FOUND, file_path)

        return Error.success()

    def process(self) -> Error:
        input_file = self.context.args.input_file
        output_file = self.context.args.output_file

        byteClassOffset = sys.getsizeof(b"") #this is the offset depending on python byte class
        split_16k = 16384 #split by 16kb as client wanted
        splitSize = split_16k + byteClassOffset #offset 17 or 33 bytes due to python byte class

        compress_data = b''
        byteRead = b''
        with open(input_file, 'rb') as fr:
            byteRead = fr.read(1)
            currByte = 0
            fileNum = 0
            toWrite = b""
            LZMA_Size = []
            if byteRead != b"":
                currByte = currByte + 1
                toWrite = toWrite + byteRead

            #print("Splitting " + fileName + " into " + str(splitSize) + " bytes per file, then compress into *.lzma files")
            ###if the current read byte is not NULL, this will run
            while byteRead != b"":
                byteRead = fr.read(1) #read next byte
                toWrite = toWrite + byteRead
                currByte = sys.getsizeof(toWrite)

                if currByte == splitSize or byteRead == b"":
                    tmp = lzma.compress(toWrite, format=lzma.FORMAT_ALONE, preset=9)
                    compress_data = compress_data + tmp
                    LZMA_Size.append(len(tmp))

                    toWrite = b''
                    currByte = 0
                    fileNum = fileNum + 1
        headerFile = fileNum.to_bytes(2, 'little')
        for item in range(0, fileNum):
            headerFile = headerFile + LZMA_Size[item].to_bytes(2, 'little')

        image_temp = os.path.splitext(input_file)[0] + '_compress_tmp2.bin'
        with open(image_temp, 'wb') as fw:
            fw.write(headerFile)
            fw.write(compress_data)

        res = self.manifest_manager.create_manifest(output_file, image_temp, img_type=parse_image_type(input_file), compress=True)
        if res:
            return res

        append_files(output_file, image_temp)
        return Error.success()

    def post_process(self) -> Error:
        return Error.success()


