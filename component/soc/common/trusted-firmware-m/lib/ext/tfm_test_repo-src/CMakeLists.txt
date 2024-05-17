#-------------------------------------------------------------------------------
# Copyright (c) 2021-2023, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

add_subdirectory(log)

add_subdirectory(lib/ext)

if(NS)
    add_subdirectory(app)
endif()

add_subdirectory(test)
