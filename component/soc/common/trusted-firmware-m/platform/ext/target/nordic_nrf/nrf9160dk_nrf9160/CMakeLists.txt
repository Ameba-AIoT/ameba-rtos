#-------------------------------------------------------------------------------
# Copyright (c) 2020, Nordic Semiconductor ASA.
# Copyright (c) 2022, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

cmake_policy(SET CMP0076 NEW)
set(CMAKE_CURRENT_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR})
set(NRF_BOARD_SELECTED True)

add_subdirectory(../common/nrf9160 nrf9160)

target_include_directories(platform_region_defs
    INTERFACE
        ../common/nrf9160/partition
)

target_sources(platform_s
    PRIVATE
        $<$<BOOL:${TFM_PARTITION_PLATFORM}>:${CMAKE_CURRENT_SOURCE_DIR}/services/src/tfm_platform_system.c>
)

target_include_directories(platform_s
    PUBLIC
        .
        ../common/nrf9160/partition
        services/include
)

target_include_directories(platform_ns
    PUBLIC
        .
)

if(BL2)
    target_include_directories(platform_bl2
        PUBLIC
            ../common/nrf9160/partition
        PRIVATE
            .
    )
endif()

if (TFM_PARTITION_PLATFORM)
install(FILES       services/include/tfm_ioctl_api.h
        DESTINATION ${TFM_INSTALL_PATH}/interface/include)
endif()

#========================= tfm_spm ============================================#

target_sources(tfm_spm
    PRIVATE
        tfm_hal_platform.c
)
