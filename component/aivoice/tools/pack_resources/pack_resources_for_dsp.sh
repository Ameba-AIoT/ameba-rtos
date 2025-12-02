#!/bin/bash

# use menuconfig to select resources
# 1. run menuconfig under amebalite_gcc_project,
#    first enable DSP, then enable AIVoice and select resources.
# 2. run this script, then aivoice_models.bin is generated under current directory

CURRENT_DIR=`pwd`
SCRIPT_DIR=$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)
AIVOICE_BINS=${SCRIPT_DIR}/../../prebuilts/bin
SDK_CONFIG=${SCRIPT_DIR}/../../../../amebalite_gcc_project/menuconfig/.config

echo "use menuconfig to generate aivoice binary"
python "${SCRIPT_DIR}/aivoice_bin_packer.py" \
    --config ${SDK_CONFIG} \
    --bins_dir ${AIVOICE_BINS} \
    --out_dir ${CURRENT_DIR}