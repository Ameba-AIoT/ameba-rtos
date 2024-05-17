#-------------------------------------------------------------------------------
# Copyright (c) 2023, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

set(ERPC_REPO_PATH   "DOWNLOAD"  CACHE PATH      "Path to eRPC repo (or DOWNLOAD to fetch automatically")
set(ERPC_VERSION     "1.9.1"     CACHE STRING    "The version of eRPC to use")

fetch_remote_library(
    LIB_NAME                erpc
    LIB_SOURCE_PATH_VAR     ERPC_REPO_PATH
    LIB_BASE_DIR            "${CMAKE_BINARY_DIR}/lib/ext"
    FETCH_CONTENT_ARGS
        GIT_REPOSITORY      https://github.com/EmbeddedRPC/erpc.git
        GIT_TAG             ${ERPC_VERSION}
        GIT_SHALLOW         TRUE
        GIT_PROGRESS        TRUE
        GIT_SUBMODULES      ""
)
