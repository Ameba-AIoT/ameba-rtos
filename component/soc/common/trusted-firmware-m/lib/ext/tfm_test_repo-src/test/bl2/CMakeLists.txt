#-------------------------------------------------------------------------------
# Copyright (c) 2022, Arm Limited. All rights reserved.
# Copyright (c) 2022 Cypress Semiconductor Corporation (an Infineon company)
# or an affiliate of Cypress Semiconductor Corporation. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.13)

if(NOT TEST_BL2 OR NOT BL2)
    return()
endif()

# Reload the compiler options for the secure core, if the NS core differs
if (EXISTS ${TARGET_PLATFORM_PATH}/preload_ns.cmake)
    include(${TARGET_PLATFORM_PATH}/preload.cmake)
    tfm_toolchain_reload_compiler()
endif()

add_subdirectory(mcuboot)
