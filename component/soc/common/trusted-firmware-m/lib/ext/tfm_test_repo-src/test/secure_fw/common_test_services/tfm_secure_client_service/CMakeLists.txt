#-------------------------------------------------------------------------------
# Copyright (c) 2020-2022, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

cmake_policy(SET CMP0079 NEW)

add_library(tfm_psa_rot_partition_secure_client_service STATIC)

target_sources(tfm_psa_rot_partition_secure_client_service
    PRIVATE
        ./tfm_secure_client_service.c
)

# The generated sources
target_sources(tfm_psa_rot_partition_secure_client_service
    PRIVATE
        ${CMAKE_BINARY_DIR}/generated/secure_fw/test_services/tfm_secure_client_service/auto_generated/intermedia_tfm_secure_client_service.c
)

target_sources(tfm_partitions
    INTERFACE
        ${CMAKE_BINARY_DIR}/generated/secure_fw/test_services/tfm_secure_client_service/auto_generated/load_info_tfm_secure_client_service.c
)

# Set include directory
target_include_directories(tfm_psa_rot_partition_secure_client_service
    PUBLIC
        .
    PRIVATE
        ${CMAKE_BINARY_DIR}/generated/secure_fw/test_services/tfm_secure_client_service
)

target_include_directories(tfm_partitions
    INTERFACE
        ${CMAKE_BINARY_DIR}/generated/secure_fw/test_services/tfm_secure_client_service
)

target_link_libraries(tfm_psa_rot_partition_secure_client_service
    PRIVATE
        tfm_sprt
        platform_s
        tfm_test_framework_s
        tfm_s_tests
)

target_link_libraries(tfm_spm
    PRIVATE
        tfm_psa_rot_partition_secure_client_service
)

############################ Partition Defs ####################################

target_link_libraries(tfm_partitions
    INTERFACE
        tfm_psa_rot_partition_secure_client_service
)

target_compile_definitions(tfm_partition_defs
    INTERFACE
        TFM_PARTITION_TEST_SECURE_SERVICES
)
