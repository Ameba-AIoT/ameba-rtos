#-------------------------------------------------------------------------------
# Copyright (c) 2022, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

add_definitions(
    -DCORSTONE310_FVP
)

set(CORSTONE310_COMMON_DIR "${CMAKE_CURRENT_LIST_DIR}/../common")

include(${CORSTONE310_COMMON_DIR}/preload.cmake)
