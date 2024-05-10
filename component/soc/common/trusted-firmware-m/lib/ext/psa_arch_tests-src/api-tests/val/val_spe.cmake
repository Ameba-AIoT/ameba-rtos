#/** @file
# * Copyright (c) 2019-2020, Arm Limited or its affiliates. All rights reserved.
# * SPDX-License-Identifier : Apache-2.0
# *
# * Licensed under the Apache License, Version 2.0 (the "License");
# * you may not use this file except in compliance with the License.
# * You may obtain a copy of the License at
# *
# *  http://www.apache.org/licenses/LICENSE-2.0
# *
# * Unless required by applicable law or agreed to in writing, software
# * distributed under the License is distributed on an "AS IS" BASIS,
# * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# * See the License for the specific language governing permissions and
# * limitations under the License.
#**/

# Listing all the sources from val
list(APPEND VAL_SRC_C_SPE
	${PSA_ROOT_DIR}/val/spe/val_driver_service_apis.c
)

# Listing common sources from partition
list(APPEND PARTITION_COMMON_SRC_C_SPE
	${PSA_ROOT_DIR}/ff/partition/driver_partition.c
)

# Listing sources from partition for client
list(APPEND PARTITION_IPC_CLIENT_SRC_C_SPE
	${PSA_ROOT_DIR}/ff/partition/client_partition.c
)
# Listing sources from partition for server
list(APPEND PARTITION_IPC_SERVER_SRC_C_SPE
	${PSA_ROOT_DIR}/ff/partition/server_partition.c
)

foreach(src_file ${SUITE_CC_SOURCE_SPE})
	get_filename_component(FILE_NAME ${src_file} NAME)
	string(SUBSTRING ${FILE_NAME} 0 9 TEST_STR)
	if(${TEST_STR} STREQUAL "test_supp")
		list(APPEND SUITE_SERVER_CC_SRC_SPE ${src_file})
	else()
		list(APPEND SUITE_CLIENT_CC_SRC_SPE ${src_file})
	endif()
endforeach()

# Create Driver partition library
add_library(${PSA_TARGET_DRIVER_PARTITION_LIB} STATIC ${VAL_SRC_C_SPE} ${PARTITION_COMMON_SRC_C_SPE} ${PAL_SRC_C_DRIVER_SP})

# Create Client partition library
add_library(${PSA_TARGET_CLIENT_PARTITION_LIB} STATIC ${PARTITION_IPC_CLIENT_SRC_C_SPE} ${SUITE_CLIENT_CC_SRC_SPE})

# Create Server partition library
add_library(${PSA_TARGET_SERVER_PARTITION_LIB} STATIC ${PARTITION_IPC_SERVER_SRC_C_SPE} ${SUITE_SERVER_CC_SRC_SPE})

target_include_directories(${PSA_TARGET_DRIVER_PARTITION_LIB} PRIVATE
	${CMAKE_CURRENT_BINARY_DIR}
	${PSA_ROOT_DIR}/val/common
	${PSA_ROOT_DIR}/val/spe
	${PSA_ROOT_DIR}/ff/partition
	${PSA_ROOT_DIR}/platform/targets/common/nspe
	${PSA_ROOT_DIR}/platform/targets/${TARGET}/nspe
)

# Include paths from platform for driver partition build
foreach(inc_path ${PAL_DRIVER_INCLUDE_PATHS})
	target_include_directories(${PSA_TARGET_DRIVER_PARTITION_LIB} PRIVATE ${inc_path})
endforeach()

target_include_directories(${PSA_TARGET_CLIENT_PARTITION_LIB} PRIVATE
	${CMAKE_CURRENT_BINARY_DIR}
	${PSA_ROOT_DIR}/val/common
	${PSA_ROOT_DIR}/val/nspe
	${PSA_ROOT_DIR}/val/spe
	${PSA_ROOT_DIR}/platform/targets/common/nspe
	${PSA_ROOT_DIR}/platform/targets/${TARGET}/nspe
	${PSA_ROOT_DIR}/ff/partition
)
target_include_directories(${PSA_TARGET_SERVER_PARTITION_LIB} PRIVATE
	${CMAKE_CURRENT_BINARY_DIR}
	${PSA_ROOT_DIR}/val/common
	${PSA_ROOT_DIR}/val/spe
	${PSA_ROOT_DIR}/platform/targets/common/nspe
	${PSA_ROOT_DIR}/platform/targets/${TARGET}/nspe
	${PSA_ROOT_DIR}/ff/partition
)

# PSA Include directories
foreach(psa_inc_path ${PSA_INCLUDE_PATHS})
	target_include_directories(${PSA_TARGET_DRIVER_PARTITION_LIB} PRIVATE ${psa_inc_path})
	target_include_directories(${PSA_TARGET_CLIENT_PARTITION_LIB} PRIVATE ${psa_inc_path})
	target_include_directories(${PSA_TARGET_SERVER_PARTITION_LIB} PRIVATE ${psa_inc_path})
endforeach()
