import os
import sys
import lzma
import argparse
import subprocess
import re

def CATFILE(header, dst, *src):
    with open(dst, "wb") as dst_fd:
        if header != '':
            dst_fd.write(header)
        for isrc in src:
            if os.path.exists(isrc):
                with open(isrc, "rb") as f:
                    content = f.read()
                    dst_fd.write(content)

lproject = re.findall(r"ameba[A-Za-z0-9]+_gcc_project", os.getcwd())
if lproject == None or len(lproject[0]) == 0:
    sys.exit()

index = os.getcwd().find(lproject[0])
pwd = os.path.join(os.getcwd()[0: index], lproject[0])

parser = argparse.ArgumentParser()
parser.add_argument("target_dir",
                    help="target directory from current directory, together with the name of input firmware",
                    type=str)
args = parser.parse_args() #for testing, mask this

elf2bin_path = os.path.join(pwd, 'project_hp', 'asdk', 'gnu_utility', 'image_tool')
platform_type = sys.platform
if platform_type == "cygwin":
    elf2bin = os.path.join(elf2bin_path,"elf2bin.exe")
elif platform_type == "linux":
    elf2bin = os.path.join(elf2bin_path,"./elf2bin")
else:
    print("Error: Only supported cygwin and linux for now")
    sys.exit()

IMAGE_FULLNAME = args.target_dir
IMAGE_FILENAME = os.path.basename(IMAGE_FULLNAME)
MANIFEST_JSON = os.path.join(pwd, 'manifest.json')
IMG_DIR = os.path.dirname(IMAGE_FULLNAME)

image_compress = os.path.join(IMG_DIR, os.path.splitext(IMAGE_FILENAME)[0] + '_compress.bin')
image_temp = os.path.join(IMG_DIR, os.path.splitext(IMAGE_FILENAME)[0] + '_compress_tmp.bin')
manifest = os.path.join(IMG_DIR, 'manifest.bin')

byteClassOffset = sys.getsizeof(b"") #this is the offset depending on python byte class
split_16k = 16384 #split by 16kb as client wanted
splitSize = split_16k + byteClassOffset #offset 17 or 33 bytes due to python byte class                    

compress_data = b''
byteRead = b''
with open(IMAGE_FULLNAME, 'rb') as fr:
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

print("Header creation complete")
print("Total number of " + str(fileNum) + " x *.bin and *.lzma files generated")
############# Perform LZMA Concatenation ###############
'''
1.) Reads all lzma files according to size pointed by (headerName)
2.) Generates total lzma file (concatLzmaFile) by combining (headerName) in front
'''

with open(image_temp, 'wb') as fw:
    fw.write(headerFile)
    fw.write(compress_data)

subprocess.run([elf2bin + " manifest " + MANIFEST_JSON + " " + MANIFEST_JSON + " " + image_temp + " " + manifest + " app"],
                shell=True)
CATFILE('', image_compress, manifest, image_temp)
os.remove(image_temp)
print("Generated concatenated Lzma File %s success "%str(image_compress))