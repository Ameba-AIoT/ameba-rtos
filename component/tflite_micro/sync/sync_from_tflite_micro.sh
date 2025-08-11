#!/usr/bin/env bash
# Copyright 2021 The TensorFlow Authors. All Rights Reserved.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
# ==============================================================================
#
# Creates the project file distributions for the TensorFlow Lite Micro test and
# example targets aimed at embedded platforms.
trap 'rm -rf "$TEMP_DIR"' EXIT
#set -e -x

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR="${SCRIPT_DIR}/.."
TFLITE_LIB_DIR="${ROOT_DIR}/"
DOWNLOADS_DIR="${TFLITE_LIB_DIR}/downloads"
cd "${TFLITE_LIB_DIR}"

TEMP_DIR=$(mktemp -p $SCRIPT_DIR -d)
EXT_LIDS_DIR="${TEMP_DIR}/tflite-micro/tensorflow/lite/micro/tools/make/ext_libs"

cd "${TEMP_DIR}"

echo Cloning tflite-micro repo to "${TEMP_DIR}"
git clone --depth 1 --single-branch "https://github.com/tensorflow/tflite-micro.git"
cd tflite-micro

# Record git commit ID to version.txt
echo "TFLM version information:" > "${TFLITE_LIB_DIR}/versions.txt"
git rev-parse HEAD >> "${TFLITE_LIB_DIR}/versions.txt"

# Create the TFLM base tree
echo create_tflm_tree...
python3 tensorflow/lite/micro/tools/project_generation/create_tflm_tree.py \
  -e hello_world -e micro_speech "${TEMP_DIR}/tflm-out"

echo update tflm...
# Backup `micro/ameba-iot` `micro/kernels/ameba-iot` directory to new tree
cp -r "${TFLITE_LIB_DIR}"/tensorflow/lite/micro/ameba-aiot \
  "${TEMP_DIR}"/tflm-out/tensorflow/lite/micro/
cp -r "${TFLITE_LIB_DIR}"/tensorflow/lite/micro/kernels/ameba-aiot \
  "${TEMP_DIR}"/tflm-out/tensorflow/lite/micro/kernels/

cp -r "${TEMP_DIR}"/tflite-micro/tensorflow/lite/micro/kernels/{cmsis_nn,xtensa} \
  "${TEMP_DIR}"/tflm-out/tensorflow/lite/micro/kernels/

cd "${TFLITE_LIB_DIR}"
rm -rf tensorflow
rm -rf third_party
rm -rf signal
mv "${TEMP_DIR}/tflm-out/tensorflow" tensorflow

# update tflm_unittest
echo update tflm_unittest...
./sync/extract_tflm_unittest_srcs.sh "${TEMP_DIR}"/tflite-micro tflm_unittest_new/

cp -r "${TFLITE_LIB_DIR}"/tflm_unittest/tensorflow/lite/micro/kernels/ameba-aiot "${TFLITE_LIB_DIR}"/tflm_unittest_new/tensorflow/lite/micro/kernels/
cp -r "${TFLITE_LIB_DIR}"/tflm_unittest/tensorflow/lite/micro/testing/ameba-aiot "${TFLITE_LIB_DIR}"/tflm_unittest_new/tensorflow/lite/micro/testing/

rm -rf tflm_unittest
mv tflm_unittest_new tflm_unittest

# For this repo we are forking both the models and the examples.
rm -rf tensorflow/lite/micro/models
mkdir -p third_party/
cp -r "${TEMP_DIR}"/tflm-out/third_party/* third_party/
mkdir -p signal/
cp -r "${TEMP_DIR}"/tflm-out/signal/* signal/

echo sync cmsis_nn version...
# Sync xa_nnlib and cmsis_nn version
# update cmsis_nn version
old_cmsis_nn_prefix=`grep "set(ZIP_PREFIX_NN" ${TFLITE_LIB_DIR}/cmsis-nn.cmake | \
                    awk -F'["]' '{print $2}'`
new_cmsis_nn_prefix=`grep "ZIP_PREFIX_NN=" ${EXT_LIDS_DIR}/cmsis_nn_download.sh | \
                    awk -F'["]' '{print $2}'`
sed -i "s/${old_cmsis_nn_prefix}/${new_cmsis_nn_prefix}/g" ${TFLITE_LIB_DIR}/cmsis-nn.cmake

# Update cmsis_nn md5
old_cmsis_nn_md5=`grep "set(CMSIS_NN_MD5" ${TFLITE_LIB_DIR}/cmsis-nn.cmake | \
            awk -F'["]' '{print $2}'`
new_cmsis_nn_md5=`grep "CMSIS_NN_MD5=" ${EXT_LIDS_DIR}/cmsis_nn_download.sh | \
            awk -F'["]' '{print $2}'`
# sed -i "s/${old_cmsis_nn_md5}/${new_cmsis_nn_md5}/g" ${TFLITE_LIB_DIR}/CMakeLists.txt
sed -i "s/${old_cmsis_nn_md5}/${new_cmsis_nn_md5}/g" ${TFLITE_LIB_DIR}/cmsis-nn.cmake

# Update nnlib-hifi5 version
echo sync nnlib-hifi5 version...
old_lib_url=`grep "set(LIBRARY_URL" ${TFLITE_LIB_DIR}/CMakeLists.txt | \
            awk -F'["]' '{print $2}'`
old_lib_md5=`grep "set(LIBRARY_MD5" ${TFLITE_LIB_DIR}/CMakeLists.txt | \
            awk -F'["]' '{print $2}'`
new_lib_url=`grep "nnlib-hifi5" --after-context=2 ${EXT_LIDS_DIR}/xtensa_download.sh | \
                grep "LIBRARY_URL" | awk -F'["]' '{print $2}'`
new_lib_md5=`grep "nnlib-hifi5" --after-context=2 ${EXT_LIDS_DIR}/xtensa_download.sh | \
                grep "LIBRARY_MD5" | awk -F'["]' '{print $2}'`
echo "old:" $old_lib_url
echo "new:" $new_lib_url
sed -i "s|${old_lib_url}|${new_lib_url}|g" ${TFLITE_LIB_DIR}/CMakeLists.txt
sed -i "s/${old_lib_md5}/${new_lib_md5}/g" ${TFLITE_LIB_DIR}/CMakeLists.txt

[[ -d "${DOWNLOADS_DIR}" ]] && rm -rf "${DOWNLOADS_DIR}"

# rm -rf "${TEMP_DIR}"
