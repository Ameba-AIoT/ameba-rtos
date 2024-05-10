#-------------------------------------------------------------------------------
# Copyright (c) 2020-2022, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------
if (TFM_S_REG_TEST)
    # Test configuration: host images are not needed and work only with isolation level 1
    set(MCUBOOT_IMAGE_NUMBER                1          CACHE STRING   "Whether to combine S and NS into either 1 image, or sign each separately" FORCE)
    set(TFM_ISOLATION_LEVEL                 1          CACHE STRING   "Isolation level" FORCE)
endif()
