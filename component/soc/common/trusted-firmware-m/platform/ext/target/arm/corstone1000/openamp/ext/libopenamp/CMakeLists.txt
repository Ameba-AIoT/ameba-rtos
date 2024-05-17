#-------------------------------------------------------------------------------
# Copyright (c) 2020-2022, Arm Limited. All rights reserved.
# Copyright (c) 2022 Cypress Semiconductor Corporation (an Infineon company)
# or an affiliate of Cypress Semiconductor Corporation. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

fetch_remote_library(
    LIB_NAME                libopenamp
    LIB_SOURCE_PATH_VAR     LIBOPENAMP_SRC_PATH
    LIB_BINARY_PATH_VAR     LIBOPENAMP_BIN_PATH
    FETCH_CONTENT_ARGS
        GIT_TAG             ${OPENAMP_VERSION}
        GIT_REPOSITORY      https://github.com/OpenAMP/open-amp.git
)

if (NOT LIB_BINARY_PATH_VAR)
set(LIBOPENAMP_BIN_PATH "${CMAKE_SOURCE_DIR}/build/lib/ext/libopenamp-subbuild" CACHE PATH "Path to build directory of open-amp.")
endif()
