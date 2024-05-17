#-------------------------------------------------------------------------------
# Copyright (c) 2020-2022, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

if (NOT TFM_PARTITION_EXAMPLE)
    return()
endif()

cmake_minimum_required(VERSION 3.15)
cmake_policy(SET CMP0079 NEW)

add_library(tfm_psa_rot_partition_example STATIC)

target_sources(tfm_psa_rot_partition_example PRIVATE
    partition_example.c
)

# The generated sources
target_sources(tfm_psa_rot_partition_example
    PRIVATE
        ${CMAKE_BINARY_DIR}/generated/secure_fw/partitions/partition_example/auto_generated/intermedia_tfm_partition_example.c
)
target_sources(tfm_partitions
    INTERFACE
        ${CMAKE_BINARY_DIR}/generated/secure_fw/partitions/partition_example/auto_generated/load_info_tfm_partition_example.c
)

# Set include directory
target_include_directories(tfm_psa_rot_partition_example
    PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        ${CMAKE_BINARY_DIR}/generated/secure_fw/partitions/partition_example
)
target_include_directories(tfm_partitions
    INTERFACE
        ${CMAKE_BINARY_DIR}/generated/secure_fw/partitions/partition_example
)

target_link_libraries(tfm_psa_rot_partition_example
    PRIVATE
        platform_s
        tfm_config
        tfm_sprt
)

############################ Partition Defs ####################################

target_link_libraries(tfm_partitions
    INTERFACE
        tfm_psa_rot_partition_example
)

target_compile_definitions(tfm_partition_defs
    INTERFACE
        TFM_PARTITION_EXAMPLE
)
