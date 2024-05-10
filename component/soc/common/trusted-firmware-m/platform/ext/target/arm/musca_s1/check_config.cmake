#-------------------------------------------------------------------------------
# Copyright (c) 2022, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

if (TEST_S_FPU OR TEST_NS_FPU)
    message(FATAL_ERROR "FPU tests are not supported on Musca-S1 platform.")
endif()
