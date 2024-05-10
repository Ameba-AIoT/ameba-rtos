#-------------------------------------------------------------------------------
# Copyright (c) 2022, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

## test incompatible compiler
tfm_invalid_config(CMAKE_C_COMPILER MATCHES "armclang" AND CMAKE_C_COMPILER_VERSION VERSION_LESS 6.18.0)
