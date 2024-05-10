#-------------------------------------------------------------------------------
# Copyright (c) 2022, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.15)

Project("CPAK Generator" LANGUAGES C)

add_executable(cpak_generator)

set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)
set(CMAKE_POLICY_DEFAULT_CMP0048 NEW)
set(ENABLE_TESTING OFF)
set(ENABLE_PROGRAMS OFF)
set(MBEDTLS_FATAL_WARNINGS OFF)
set(ENABLE_DOCS OFF)
set(INSTALL_MBEDTLS_HEADERS OFF)

add_subdirectory(${MBEDCRYPTO_PATH} mbedtls)

target_sources(cpak_generator
    PRIVATE
        ./cpak_generator.c
)

target_link_libraries(cpak_generator
    PRIVATE
        mbedtls
)

target_compile_definitions(cpak_generator
    PRIVATE
        BL1_2_HASH_PATH=${BL1_2_HASH_PATH}
        GUK_PATH=${GUK_PATH}
        CPAK_OUTPUT_PATH=${CPAK_OUTPUT_PATH}
)

target_compile_options(cpak_generator
    PRIVATE
        -Og
        -g
)
