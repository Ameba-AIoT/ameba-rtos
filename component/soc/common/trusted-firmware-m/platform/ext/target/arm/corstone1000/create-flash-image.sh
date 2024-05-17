#!/bin/bash
#-------------------------------------------------------------------------------
# Copyright (c) 2023, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

######################################################################
# This script is to create a flash gpt image for corstone platform
# 
#  Flash image layout:
#       |------------------------------|
#       |        Protective MBR        |
#       |------------------------------|
#       |          GPT Header          |
#       |------------------------------|
#       |          reserved_1          |
#       |------------------------------|
#       |         FWU-Metadata         |
#       |------------------------------|
#       |      Bkup-FWU-Metadata       |
#       |------------------------------|
#       |  private_metadata_replica_1  |
#       |------------------------------|
#       |  private_metadata_replica_2  |
#       |------------------------------|
#       |         bl2_primary          |
#       |------------------------------|
#       |         tfm_primary          |
#       |------------------------------|
#       |                              |
#       |                              |
#       |------------------------------|
#       |        bl2_secondary         |
#       |------------------------------|
#       |        tfm_secondary         |
#       |------------------------------|
#       |                              |
#       |                              |
#       |------------------------------|
#       |         reserved_2           |
#       |------------------------------|
#       |       BKUP GPT Header        |
#       |------------------------------|
######################################################################


# Path to build directory that contains binaries must be provided
if [ -z "$1" ]; then
  echo "Error: Build directory path argument must be provided."
  echo "Usage: $0 [build directory path] [output image name (default is cs1000.bin)]"
  exit 1
fi

# Set build directory
BIN_DIR=$1
# Set output image
IMAGE=$BIN_DIR/${2:-cs1000.bin}
# Set the image  size
IMAGE_SIZE=32M

# Set Type UUIDs
FWU_METADATA_TYPE_UUID="8A7A84A0-8387-40F6-AB41-A8B9A5A60D23"
PRIVATE_METADATA_TYPE_UUID="ECB55DC3-8AB7-4A84-AB56-EB0A9974DB42"
SE_BL2_TYPE_UUID="64BD8ADB-02C0-4819-8688-03AB4CAB0ED9"
TFM_TYPE_UUID="D763C27F-07F6-4FF0-B2F3-060CB465CD4E"

# Create the image
rm -f $IMAGE
echo "remove old image if exists"
(dd if=/dev/zero of=$IMAGE bs=$IMAGE_SIZE count=1 && echo "Image has been created" ) ||\
(echo "something goes wrong while creating the image" && exit 1)

# Create the image using sgdisk
sgdisk  --mbrtogpt \
        --clear \
        --set-alignment 1 \
        --new=1:34:+3k --partition-guid=1:$(uuidgen) --change-name=1:'reserved_1' \
        --new=2:40:+4K --typecode=2:$FWU_METADATA_TYPE_UUID --partition-guid=2:$(uuidgen) --change-name=2:'FWU-Metadata' \
        --new=3:48:+4K --typecode=3:$FWU_METADATA_TYPE_UUID --partition-guid=3:$(uuidgen) --change-name=3:'Bkup-FWU-Metadata' \
        --new=4:56:+4K --typecode=4:$PRIVATE_METADATA_TYPE_UUID --partition-guid=4:$(uuidgen) --change-name=4:'private_metadata_replica_1' \
        --new=5:64:+4k --typecode=5:$PRIVATE_METADATA_TYPE_UUID --partition-guid=5:$(uuidgen) --change-name=5:'private_metadata_replica_2' \
        --new=6:72:+100k --typecode=6:$SE_BL2_TYPE_UUID --partition-guid=6:$(uuidgen) --change-name=6:'bl2_primary' \
        --new=7:272:+376K --typecode=7:$TFM_TYPE_UUID --partition-guid=7:$(uuidgen) --change-name=7:'tfm_primary' \
        --new=8:32784:+100k --typecode=8:$SE_BL2_TYPE_UUID --partition-guid=8:$(uuidgen) --change-name=8:'bl2_secondary' \
        --new=9:32984:+376K --typecode=9:$TFM_TYPE_UUID --partition-guid=9:$(uuidgen) --change-name=9:'tfm_secondary' \
        --new=10:65496:65501  --partition-guid=10:$(uuidgen) --change-name=10:'reserved_2' \
        $IMAGE

[ $? -ne 0 ] && echo "Error occurs while writing the GPT layout" && exit 1

# Write partitions 
# conv=notrunc avoids truncation to keep the geometry of the image.
dd if=$BIN_DIR/bl2_signed.bin of=${IMAGE}  seek=72 conv=notrunc
dd if=$BIN_DIR/tfm_s_signed.bin of=${IMAGE} seek=272 conv=notrunc

# Print the gpt table
sgdisk -p $IMAGE
