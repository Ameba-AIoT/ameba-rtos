#-------------------------------------------------------------------------------
# Copyright (c) 2021-2022 Cypress Semiconductor Corporation (an Infineon company)
# or an affiliate of Cypress Semiconductor Corporation. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

fetch_remote_library(
    LIB_NAME                p64_utils
    LIB_SOURCE_PATH_VAR     CY_P64_UTILS_LIB_PATH
    LIB_PATCH_DIR           ${CMAKE_CURRENT_LIST_DIR}
    LIB_FORCE_PATCH         CY_P64_UTILS_FORCE_PATCH
    FETCH_CONTENT_ARGS
        GIT_REPOSITORY      https://github.com/Infineon/p64_utils.git
        GIT_TAG             ${CY_P64_UTILS_LIB_VERSION}
        GIT_SHALLOW         TRUE
        GIT_PROGRESS        TRUE
        GIT_SUBMODULES      ""
)

#-------------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.13)

project("Cypress PSoC64 Utils Library" LANGUAGES C ASM)

cmake_policy(SET CMP0076 NEW)
cmake_policy(SET CMP0079 NEW)

if (NOT DEFINED P64_UTILS_LIB_TARGET)
    set(P64_UTILS_LIB_TARGET p64_utils_lib)
endif()

add_definitions(-DCY_P64_FREE_WIPED)

add_library(${P64_UTILS_LIB_TARGET} STATIC)

target_sources(${P64_UTILS_LIB_TARGET}
    PRIVATE
        ${CY_P64_UTILS_LIB_PATH}/cy_p64_base64.c
        ${CY_P64_UTILS_LIB_PATH}/cy_p64_cJSON.c
        ${CY_P64_UTILS_LIB_PATH}/cy_p64_jwt_policy.c
        ${CY_P64_UTILS_LIB_PATH}/cy_p64_malloc.c
        ${CY_P64_UTILS_LIB_PATH}/cy_p64_syscalls.c
        ${CY_P64_UTILS_LIB_PATH}/cy_p64_syscall.c
        ${CY_P64_UTILS_LIB_PATH}/cy_p64_watchdog.c
        ${CY_P64_UTILS_LIB_PATH}/cy_p64_image.c
        ${CY_P64_UTILS_LIB_PATH}/cy_p64_psacrypto.c
)

target_include_directories(${P64_UTILS_LIB_TARGET}
    PUBLIC
        ${CY_P64_UTILS_LIB_PATH}
    PRIVATE
        ${CY_PSOC64_DIR}/Device/Include
        ${CY_CORE_LIB_PATH}/include
        ${CY_MTB_PDL_CAT1_LIB_PATH}/cmsis/include
        ${CY_MTB_PDL_CAT1_LIB_PATH}/devices/COMPONENT_CAT1A/include
        ${CY_MTB_PDL_CAT1_LIB_PATH}/drivers/include
)

