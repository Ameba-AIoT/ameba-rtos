#!/usr/bin/env python3
# -*- coding:utf-8 -*- 
import os
import sys


def pad(filename, alignment):
    # Get Parameters
    IMAGE_FILENAME=filename
    Aligned_Bytes=alignment
    if os.path.exists(IMAGE_FILENAME):
        file_size = os.path.getsize(IMAGE_FILENAME)
        if file_size == 0:
            padcount = 0
        else:
            new_size = ((file_size - 1) // Aligned_Bytes + 1) * Aligned_Bytes
            padcount = new_size - file_size

        with open(IMAGE_FILENAME, "ab+") as file:
            for _ in range(padcount):
                file.write(b'\xFF')

def walk_dir(rootdir):
    files_ = []
    if os.path.exists(rootdir) == False:
        return files_
    dirlist = os.listdir(rootdir)
    for i in range(0, len(dirlist)):
        path = os.path.join(rootdir, dirlist[i])
        if os.path.isfile(path) and path.endswith('tinf.bin'):
            files_.append(path)
    return files_


def prepend_header(filename, startaddr):
    # Constant Variables
    RSVD=0xFFFFFFFFFFFFFFFF
    IMG2SIGN=0x3831393538373131
    APPSIGN=0x74696E6661707030

    # Get Parameters
    IMAGE_FILENAME=filename
    IMAGE_ADDR=startaddr

    IMAGE_LEN = os.path.getsize(IMAGE_FILENAME)

    IMAGE_FILENAME_NEW=os.path.basename(IMAGE_FILENAME)
    IMAGE_FILENAME_PREPEND = os.path.join(os.path.dirname(IMAGE_FILENAME), 
                                        os.path.splitext(IMAGE_FILENAME_NEW)[0] + '_prepend' + os.path.splitext(IMAGE_FILENAME_NEW)[1])
    # print(IMAGE_FILENAME_PREPEND)

    HEADER_FINAL=''
    if IMAGE_FILENAME.endswith('tinf.bin'):
        HEADER_FINAL = APPSIGN.to_bytes(8, 'big')
    else:
        HEADER_FINAL = IMG2SIGN.to_bytes(8, 'big')

    HEADER_FINAL = HEADER_FINAL + IMAGE_LEN.to_bytes(4, 'little') + int(IMAGE_ADDR, 16).to_bytes(4, 'little')
    HEADER_FINAL = HEADER_FINAL + RSVD.to_bytes(8, 'little')
    HEADER_FINAL = HEADER_FINAL + RSVD.to_bytes(8, 'little')

    dst_fd = open(IMAGE_FILENAME_PREPEND, 'wb')
    dst_fd.write(HEADER_FINAL)
    with open(IMAGE_FILENAME, 'rb') as f:
        content = f.read()
        dst_fd.write(content)
    dst_fd.close()

def cat_apps_bin(imagedir):
    app_images = walk_dir(imagedir)
    for image in app_images:
        pad(image, 32)
        prepend_header(image, '0x0')
    
    dst_fd = open(os.path.join(imagedir, 'dynamic_app.bin'), 'wb')
    
    for image in app_images:
        image_name = os.path.basename(image)
        image_prepend = os.path.join(os.path.dirname(image), 
                                    os.path.splitext(image_name)[0] + '_prepend' + os.path.splitext(image_name)[1])
        if os.path.exists(image_prepend):
            with open(image_prepend, "rb") as f:
                content = f.read()
                dst_fd.write(content)
    dst_fd.close()


if __name__ == '__main__':
    if len(sys.argv) <= 1:
        sys.exit()
    # print(sys.argv[1])
    if sys.argv[1] == 'imagetool':
        cat_apps_bin(sys.argv[2])
    elif sys.argv[1] == 'prepend':
        prepend_header(sys.argv[2], sys.argv[3])


