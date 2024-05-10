#-------------------------------------------------------------------------------
# Copyright (c) 2020-2022 Cypress Semiconductor Corporation (an Infineon company)
# or an affiliate of Cypress Semiconductor Corporation. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

fetch_remote_library(
    LIB_NAME                mtb-pdl-cat1
    LIB_SOURCE_PATH_VAR     CY_MTB_PDL_CAT1_LIB_PATH
    LIB_PATCH_DIR           ${CMAKE_CURRENT_LIST_DIR}
    LIB_FORCE_PATCH         CY_MTB_PDL_CAT1_FORCE_PATCH
    FETCH_CONTENT_ARGS
        GIT_REPOSITORY      https://github.com/Infineon/mtb-pdl-cat1.git
        GIT_TAG             ${CY_MTB_PDL_CAT1_LIB_VERSION}
        GIT_SHALLOW         TRUE
        GIT_PROGRESS        TRUE
        GIT_SUBMODULES      ""
)

#-------------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.13)

project("Cypress PSoC 6 Peripheral Driver Library" LANGUAGES C ASM)

cmake_policy(SET CMP0076 NEW)
cmake_policy(SET CMP0079 NEW)

if (NOT DEFINED MTB_PDL_CAT1_LIB_S_TARGET)
    set(MTB_PDL_CAT1_LIB_S_TARGET mtb-pdl-cat1_lib_s)
endif()
if (NOT DEFINED MTB_PDL_CAT1_LIB_NS_TARGET)
    set(MTB_PDL_CAT1_LIB_NS_TARGET mtb-pdl-cat1_lib_ns)
endif()

set(CY_PLATFORM_DIR         ${PLATFORM_DIR}/ext/target/${TFM_PLATFORM})
set(CY_CORE_LIB_PATH        ${CY_PLATFORM_DIR}/libs/core-lib)

add_subdirectory(mtb-pdl-cat1_s_lib)
add_subdirectory(mtb-pdl-cat1_ns_lib)
