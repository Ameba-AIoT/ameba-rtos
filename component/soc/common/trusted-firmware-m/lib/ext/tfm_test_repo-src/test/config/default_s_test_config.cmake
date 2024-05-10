#-------------------------------------------------------------------------------
# Copyright (c) 2021-2022, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

########################## S test framework ####################################

set(TEST_FRAMEWORK_S        ON        CACHE BOOL      "Whether to build S regression tests framework")

########################## S test suites #######################################

set(TEST_S_ATTESTATION      ON        CACHE BOOL      "Whether to build S regression Attestation tests")
set(TEST_S_CRYPTO           ON        CACHE BOOL      "Whether to build S regression Crypto tests")
set(TEST_S_ITS              ON        CACHE BOOL      "Whether to build S regression ITS tests")
set(TEST_S_PS               ON        CACHE BOOL      "Whether to build S regression PS tests")
set(TEST_S_PLATFORM         ON        CACHE BOOL      "Whether to build S regression Platform tests")

set(TEST_S_FWU              OFF       CACHE BOOL      "Whether to build S regression FWU tests")
set(TEST_S_IPC              OFF       CACHE BOOL      "Whether to build S regression IPC tests")
set(TEST_S_SFN_BACKEND      OFF       CACHE BOOL      "Whether to build S regression SFN tests")
set(TEST_S_FPU              OFF       CACHE BOOL      "Whether to build S regression FPU tests")
