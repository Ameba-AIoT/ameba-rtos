#-------------------------------------------------------------------------------
# Copyright (c) 2023, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

cmake_policy(SET CMP0076 NEW)

include(${CORSTONE310_COMMON_DIR}/common.cmake)

add_library(flash_drivers STATIC)

target_include_directories(flash_drivers
    PRIVATE
        ${PLATFORM_DIR}/ext/target/arm/drivers/qspi/xilinx_pg153_axi
        ${PLATFORM_DIR}/ext/target/arm/drivers/flash/sst26vf064b
        ${PLATFORM_DIR}/ext/target/arm/drivers/flash/common
        ${PLATFORM_DIR}/ext/driver
        device/include
        device/config
)

target_sources(flash_drivers
    PRIVATE
        cmsis_drivers/Driver_Flash.c
        ${PLATFORM_DIR}/ext/target/arm/drivers/flash/sst26vf064b/spi_sst26vf064b_flash_lib.c
        ${PLATFORM_DIR}/ext/target/arm/drivers/qspi/xilinx_pg153_axi/xilinx_pg153_axi_qspi_controller_drv.c
        device/source/flash_device_definition.c
)

target_link_libraries(flash_drivers
    PRIVATE
        cmsis_includes_s
)

target_link_libraries(platform_bl2
    PRIVATE
        flash_drivers
)

target_link_libraries(platform_s
    PRIVATE
        flash_drivers
)

target_link_libraries(platform_ns
    PRIVATE
        flash_drivers
)
