#-------------------------------------------------------------------------------
# Copyright (c) 2023, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

if(NOT ${CMAKE_GENERATOR} STREQUAL "Unix Makefiles" AND
   NOT ${CMAKE_GENERATOR} STREQUAL "Ninja")
    Message(FATAL_ERROR "Unsupported generator ${CMAKE_GENERATOR}. Hint: Try -G\"Unix Makefiles\"")
endif()

if(TFM_SYSTEM_DSP)
    message(FATAL_ERROR "Hardware DSP is currently not supported in TF-M")
endif()

if(TFM_LIB_MODEL)
    message(FATAL_ERROR "Library Model is deprecated, please DO NOT use TFM_LIB_MODEL anymore."
                        "SFN model is a replacement for Library Model.
                         You can use -DCONFIG_TFM_SPM_BACKEND=SFN to select SFN model.")
endif()

# The default build type is MinSizeRel. If debug symbols are needed then
# -DCMAKE_BUILD_TYPE=debug should be used (likewise with other build types)
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "MinSizeRel" CACHE STRING "Build type: [Debug, Release, RelWithDebInfo, MinSizeRel]" FORCE)
endif()

# Configure TFM_PLATFORM
include(${CMAKE_SOURCE_DIR}/config/tfm_platform.cmake)

# Some compiler flags depend on the CPU / platform config. This include should
# be run before the toolchain file so the compiler can be configured properly.
if (NOT EXISTS ${TARGET_PLATFORM_PATH}/preload.cmake)
    Message(FATAL_ERROR "preload.cmake is not found in ${TFM_PLATFORM}")
else()
    include(${TARGET_PLATFORM_PATH}/preload.cmake)
endif()
