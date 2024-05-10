#-------------------------------------------------------------------------------
# Copyright (c) 2023, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

add_definitions(
    -DCORSTONE310_AN555
)

set(CORSTONE310_COMMON_DIR "${CMAKE_CURRENT_LIST_DIR}/../common")

include(${CORSTONE310_COMMON_DIR}/preload.cmake)
