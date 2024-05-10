#-------------------------------------------------------------------------------
# Copyright (c) 2022-2023, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

cmake_policy(SET CMP0076 NEW)
set(CMAKE_CURRENT_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR})

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../common common)

#========================= Platform region defs ===============================#

target_include_directories(platform_region_defs
    INTERFACE
        .
)

#========================= Platform_s =========================================#

target_sources(platform_s
    PRIVATE
        cmsis_drivers/Driver_Flash.c
        host_drivers/cfi_drv.c
        ${PLATFORM_DIR}/ext/target/arm/drivers/flash/strata/spi_strataflashj3_flash_lib.c
        $<$<NOT:$<BOOL:${RSS_DEBUG_UART}>>:${CMAKE_CURRENT_SOURCE_DIR}/cmsis_drivers/Driver_USART.c>
        $<$<NOT:$<BOOL:${RSS_DEBUG_UART}>>:${CMAKE_CURRENT_SOURCE_DIR}/host_drivers/uart_pl011_drv.c>
        rss_comms/rss_comms_permissions_hal.c
)

target_include_directories(platform_s
    PUBLIC
        ./host_drivers
        ${PLATFORM_DIR}/ext/target/arm/drivers/flash/common
        ${PLATFORM_DIR}/ext/target/arm/drivers/flash/strata
        ${PLATFORM_DIR}/ext/target/arm/drivers/qspi/xilinx_pg153_axi
)

#========================= Platform_ns ========================================#

target_sources(platform_ns
    PRIVATE
        cmsis_drivers/Driver_Flash.c
        host_drivers/cfi_drv.c
        ${PLATFORM_DIR}/ext/target/arm/drivers/flash/strata/spi_strataflashj3_flash_lib.c
        $<$<NOT:$<BOOL:${RSS_DEBUG_UART}>>:${CMAKE_CURRENT_SOURCE_DIR}/cmsis_drivers/Driver_USART.c>
        $<$<NOT:$<BOOL:${RSS_DEBUG_UART}>>:${CMAKE_CURRENT_SOURCE_DIR}/host_drivers/uart_pl011_drv.c>
)

target_include_directories(platform_ns
    PUBLIC
        ./host_drivers
        ${PLATFORM_DIR}/ext/target/arm/drivers/flash/common
        ${PLATFORM_DIR}/ext/target/arm/drivers/flash/strata
        ${PLATFORM_DIR}/ext/target/arm/drivers/qspi/xilinx_pg153_axi
)

#========================= Platform BL2 =======================================#

target_sources(platform_bl2
    PRIVATE
        bl2/boot_hal_bl2.c
        bl2/flash_map_bl2.c
        cmsis_drivers/Driver_Flash.c
        host_drivers/cfi_drv.c
        ${PLATFORM_DIR}/ext/target/arm/drivers/flash/strata/spi_strataflashj3_flash_lib.c
        $<$<NOT:$<BOOL:${RSS_DEBUG_UART}>>:${CMAKE_CURRENT_SOURCE_DIR}/cmsis_drivers/Driver_USART.c>
        $<$<NOT:$<BOOL:${RSS_DEBUG_UART}>>:${CMAKE_CURRENT_SOURCE_DIR}/host_drivers/uart_pl011_drv.c>
)

target_include_directories(platform_bl2
    PUBLIC
        ./host_drivers
        ${PLATFORM_DIR}/ext/target/arm/drivers/flash/common
        ${PLATFORM_DIR}/ext/target/arm/drivers/flash/strata
        ${PLATFORM_DIR}/ext/target/arm/drivers/qspi/xilinx_pg153_axi
)

#========================= Platform BL1 =======================================#

target_sources(platform_bl1
    PRIVATE
        cmsis_drivers/Driver_Flash.c
        host_drivers/cfi_drv.c
        ${PLATFORM_DIR}/ext/target/arm/drivers/flash/strata/spi_strataflashj3_flash_lib.c
        $<$<AND:$<BOOL:${TFM_BL1_LOGGING}>,$<NOT:$<BOOL:${RSS_DEBUG_UART}>>>:${CMAKE_CURRENT_SOURCE_DIR}/cmsis_drivers/Driver_USART.c>
        $<$<AND:$<BOOL:${TFM_BL1_LOGGING}>,$<NOT:$<BOOL:${RSS_DEBUG_UART}>>>:${CMAKE_CURRENT_SOURCE_DIR}/host_drivers/uart_pl011_drv.c>
)

target_include_directories(platform_bl1
    PUBLIC
        ./host_drivers
        ${PLATFORM_DIR}/ext/target/arm/drivers/flash/common
        ${PLATFORM_DIR}/ext/target/arm/drivers/flash/strata
        ${PLATFORM_DIR}/ext/target/arm/drivers/qspi/xilinx_pg153_axi
)
