#-------------------------------------------------------------------------------
# Copyright (c) 2021-22, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

############ Define secure test specific cmake configurations here #############

set (TEST_FLASH_SIZE_IN_BYTES         48U  CACHE STRING   "The size of the emulated flash used in io tests")
set (TEST_FLASH_SECTOR_SIZE_IN_BYTES  16U  CACHE STRING   "The sector size of the emulated flash used in io tests")
set (TEST_FLASH_PAGE_SIZE              8U  CACHE STRING   "The page size of the emulated flash used in io tests")
set (TEST_FLASH_PROGRAM_UNIT           1U  CACHE STRING   "The program unit of the emulated flash used in io tests")
